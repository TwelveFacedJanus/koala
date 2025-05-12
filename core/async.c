#include "async.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

// Worker thread function prototype
static void* worker_thread_func(void* arg);

AsyncEventLoop* async_event_loop_init(int num_workers) {
    if (num_workers <= 0) {
        num_workers = 1; // Default to at least one worker
    }

    // Allocate memory for the event loop structure
    AsyncEventLoop* loop = (AsyncEventLoop*)malloc(sizeof(AsyncEventLoop));
    if (!loop) {
        return NULL;
    }

    // Initialize the structure
    loop->num_workers = num_workers;
    loop->running = 1;
    loop->queue_head = NULL;
    loop->queue_tail = NULL;

    // Initialize mutex and condition variable
    if (pthread_mutex_init(&loop->queue_mtx, NULL) != 0) {
        free(loop);
        return NULL;
    }

    if (pthread_cond_init(&loop->queue_cv, NULL) != 0) {
        pthread_mutex_destroy(&loop->queue_mtx);
        free(loop);
        return NULL;
    }

    // Allocate memory for worker threads
    loop->workers = (pthread_t*)malloc(sizeof(pthread_t) * num_workers);
    if (!loop->workers) {
        pthread_mutex_destroy(&loop->queue_mtx);
        pthread_cond_destroy(&loop->queue_cv);
        free(loop);
        return NULL;
    }

    // Create worker threads
    for (int i = 0; i < num_workers; i++) {
        if (pthread_create(&loop->workers[i], NULL, worker_thread_func, loop) != 0) {
            // Error creating thread, clean up already created threads
            loop->running = 0;
            pthread_cond_broadcast(&loop->queue_cv); // Wake up any threads that might be waiting
            
            // Join already created threads
            for (int j = 0; j < i; j++) {
                pthread_join(loop->workers[j], NULL);
            }
            
            // Clean up resources
            pthread_mutex_destroy(&loop->queue_mtx);
            pthread_cond_destroy(&loop->queue_cv);
            free(loop->workers);
            free(loop);
            return NULL;
        }
    }

    return loop;
}

void async_event_loop_stop(AsyncEventLoop* loop) {
    if (!loop) return;

    // Signal all workers to stop
    pthread_mutex_lock(&loop->queue_mtx);
    loop->running = 0;
    pthread_cond_broadcast(&loop->queue_cv);
    pthread_mutex_unlock(&loop->queue_mtx);

    // Join all worker threads
    for (int i = 0; i < loop->num_workers; i++) {
        pthread_join(loop->workers[i], NULL);
    }

    // Free remaining tasks in the queue
    AsyncTaskNode* current = loop->queue_head;
    while (current) {
        AsyncTaskNode* next = current->next;
        free(current);
        current = next;
    }

    // Destroy synchronization primitives and free memory
    pthread_mutex_destroy(&loop->queue_mtx);
    pthread_cond_destroy(&loop->queue_cv);
    free(loop->workers);
    free(loop);
}

AsyncTask* async_run(AsyncEventLoop* loop, FunctionPipeline* func, FunctionParams* params) {
    if (!loop || !func) return NULL;

    // Allocate and initialize AsyncTask structure
    AsyncTask* task = (AsyncTask*)malloc(sizeof(AsyncTask));
    if (!task) return NULL;

    task->ready = 0;
    task->cancel = 0;
    memset(&task->result, 0, sizeof(Result));

    // Initialize task mutex and condition variable
    if (pthread_mutex_init(&task->mtx, NULL) != 0) {
        free(task);
        return NULL;
    }

    if (pthread_cond_init(&task->cv, NULL) != 0) {
        pthread_mutex_destroy(&task->mtx);
        free(task);
        return NULL;
    }

    // Allocate and initialize queue node
    AsyncTaskNode* node = (AsyncTaskNode*)malloc(sizeof(AsyncTaskNode));
    if (!node) {
        pthread_mutex_destroy(&task->mtx);
        pthread_cond_destroy(&task->cv);
        free(task);
        return NULL;
    }

    node->func = func;
    node->params = params;
    node->task = task;
    node->next = NULL;

    // Add task to the queue
    pthread_mutex_lock(&loop->queue_mtx);
    
    if (!loop->queue_head) {
        // Queue is empty
        loop->queue_head = node;
        loop->queue_tail = node;
    } else {
        // Append to the end of the queue
        loop->queue_tail->next = node;
        loop->queue_tail = node;
    }
    
    // Signal one worker to process the new task
    pthread_cond_signal(&loop->queue_cv);
    pthread_mutex_unlock(&loop->queue_mtx);

    return task;
}

Result async_await(AsyncTask* task) {
    Result res = {0};
    if (!task) return res;

    // Wait for task to complete
    pthread_mutex_lock(&task->mtx);
    while (!task->ready) {
        pthread_cond_wait(&task->cv, &task->mtx);
    }
    
    // Copy the result
    res = task->result;
    pthread_mutex_unlock(&task->mtx);

    // Clean up task resources
    pthread_mutex_destroy(&task->mtx);
    pthread_cond_destroy(&task->cv);
    free(task);

    return res;
}

Result async_await_timeout(AsyncTask* task, int timeout_ms) {
    Result res = {0};
    if (!task) return res;
    
    // Calculate absolute timeout time
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    
    // Convert timeout from milliseconds to seconds and nanoseconds
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    
    // Handle nanosecond overflow
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000;
    }

    // Wait for task completion with timeout
    pthread_mutex_lock(&task->mtx);
    int timedout = 0;
    
    while (!task->ready) {
        int ret = pthread_cond_timedwait(&task->cv, &task->mtx, &ts);
        if (ret == ETIMEDOUT) {
            timedout = 1;
            break;
        }
    }
    
    if (timedout) {
        // Timeout occurred
        res.is_ok = 0;
        // Set error message using the correct macro from types.h
        GENERIC_SET_TYPED(res.err, "Timeout", GENERIC_TYPE_STR);
    } else {
        // Task completed within timeout
        res = task->result;
    }
    
    pthread_mutex_unlock(&task->mtx);

    // Clean up task resources
    pthread_mutex_destroy(&task->mtx);
    pthread_cond_destroy(&task->cv);
    free(task);

    return res;
}

void async_cancel(AsyncTask* task) {
    if (!task) return;
    
    pthread_mutex_lock(&task->mtx);
    task->cancel = 1;
    pthread_mutex_unlock(&task->mtx);
    
    // If task is already being processed, this flag will be checked
    // If it's still in the queue, the worker will check before processing
}

// Worker thread function
static void* worker_thread_func(void* arg) {
    AsyncEventLoop* loop = (AsyncEventLoop*)arg;
    
    while (1) {
        // Get a task from the queue
        AsyncTaskNode* node = NULL;
        
        pthread_mutex_lock(&loop->queue_mtx);
        
        // Wait until there's a task or the loop is stopping
        while (loop->queue_head == NULL && loop->running) {
            pthread_cond_wait(&loop->queue_cv, &loop->queue_mtx);
        }
        
        // Check if we should exit
        if (!loop->running && loop->queue_head == NULL) {
            pthread_mutex_unlock(&loop->queue_mtx);
            break;
        }
        
        // Dequeue a task
        if (loop->queue_head) {
            node = loop->queue_head;
            loop->queue_head = node->next;
            
            // Update tail if queue is now empty
            if (loop->queue_head == NULL) {
                loop->queue_tail = NULL;
            }
        }
        
        pthread_mutex_unlock(&loop->queue_mtx);
        
        // Process the task
        if (node) {
            AsyncTask* task = node->task;
            
            // Check for cancellation
            int cancelled = 0;
            pthread_mutex_lock(&task->mtx);
            cancelled = task->cancel;
            pthread_mutex_unlock(&task->mtx);
            
            if (!cancelled) {
                // Execute the function
                Result result = node->func->func(node->params);
                
                // Update task with result
                pthread_mutex_lock(&task->mtx);
                task->result = result;
                task->ready = 1;
                pthread_cond_broadcast(&task->cv);
                pthread_mutex_unlock(&task->mtx);
            } else {
                // Task was cancelled, mark as ready without executing
                pthread_mutex_lock(&task->mtx);
                task->ready = 1;
                pthread_cond_broadcast(&task->cv);
                pthread_mutex_unlock(&task->mtx);
            }
            
            // Free the task node (but not the task itself, which is freed after await)
            free(node);
        }
    }
    
    return NULL;
}
