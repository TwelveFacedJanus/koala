#ifndef KOALA_ASYNC_H
#define KOALA_ASYNC_H

#ifdef __cplusplus
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>
#else
#include <pthread.h>
#endif
#include "types.h"

/**
 * @defgroup Async Асинхронные примитивы
 * @brief Event loop, Future, Promise для FunctionPipeline/Result
 */

/**
 * @defgroup AsyncUsage Примеры использования асинхронных примитивов
 * @ingroup Async
 *
 * @code{.c}
 * // C: Асинхронный запуск функции из FunctionPipeline
 * #include "types.h"
 * #include "async.h"
 *
 * Result my_func(FunctionParams* params) {
 *     Result res;
 *     res.is_ok = 1;
 *     GENERIC_SET(res.ok, "Hello from async!");
 *     return res;
 * }
 *
 * int main() {
 *     AsyncEventLoop* loop = async_event_loop_init();
 *     FunctionPipeline fp = { my_func, NULL };
 *     FunctionParams params = { 0, NULL };
 *     AsyncTask* task = async_run(loop, &fp, &params);
 *     Result r = async_await(task);
 *     if (r.is_ok) printf("%s\n", (char*)GENERIC_GET(char*, r.ok));
 *     async_event_loop_stop(loop);
 *     return 0;
 * }
 * @endcode
 *
 * @code{.cpp}
 * // C++: Асинхронный запуск функции из FunctionPipeline
 * #include "types.h"
 * #include "async.h"
 * #include <iostream>
 *
 * Result my_func(FunctionParams* params) {
 *     Result res;
 *     res.is_ok = true;
 *     res.ok = Generic<const char*>("Hello from async!");
 *     return res;
 * }
 *
 * int main() {
 *     AsyncEventLoop* loop = async_event_loop_init();
 *     FunctionPipeline fp = { my_func, nullptr };
 *     FunctionParams params = { 0, nullptr };
 *     AsyncTask* task = async_run(loop, &fp, &params);
 *     Result r = async_await(task);
 *     if (r.is_ok) std::cout << static_cast<const char*>(r.ok.value) << std::endl;
 *     async_event_loop_stop(loop);
 *     return 0;
 * }
 * @endcode
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Дескриптор асинхронной задачи (future/promise)
 */
typedef struct AsyncTask {
#ifdef __cplusplus
    std::mutex mtx;
    std::condition_variable cv;
    bool ready;
    bool cancel; ///< Флаг отмены задачи
    Result result;
#else
    pthread_mutex_t mtx;
    pthread_cond_t cv;
    int ready;
    int cancel; ///< Флаг отмены задачи
    Result result;
#endif
} AsyncTask;

/**
 * @brief Узел очереди задач для event loop
 */
typedef struct AsyncTaskNode {
    FunctionPipeline* func;
    FunctionParams* params;
    AsyncTask* task;
    struct AsyncTaskNode* next;
} AsyncTaskNode;

/**
 * @brief Event loop для асинхронных задач с пулом потоков
 */
typedef struct AsyncEventLoop {
#ifdef __cplusplus
    std::vector<std::thread*> workers; ///< Пул воркеров
    int num_workers;
    std::mutex queue_mtx;
    std::condition_variable queue_cv;
    std::queue<AsyncTaskNode*> queue;
    bool running;
#else
    pthread_t* workers;
    int num_workers;
    pthread_mutex_t queue_mtx;
    pthread_cond_t queue_cv;
    AsyncTaskNode* queue_head;
    AsyncTaskNode* queue_tail;
    int running;
#endif
} AsyncEventLoop;

/**
 * @brief Инициализация event loop
 * @return Указатель на новый AsyncEventLoop
 */
AsyncEventLoop* async_event_loop_init();

/**
 * @brief Остановить event loop и освободить ресурсы
 * @param loop AsyncEventLoop*
 */
void async_event_loop_stop(AsyncEventLoop* loop);

/**
 * @brief Запустить функцию из FunctionPipeline асинхронно через event loop
 * @param loop AsyncEventLoop*
 * @param func FunctionPipeline*
 * @param params FunctionParams*
 * @return AsyncTask* дескриптор задачи
 */
AsyncTask* async_run(AsyncEventLoop* loop, FunctionPipeline* func, FunctionParams* params);

/**
 * @brief Ожидать завершения асинхронной задачи и получить Result
 * @param task AsyncTask*
 * @return Result
 */
Result async_await(AsyncTask* task);

/**
 * @brief Отменить асинхронную задачу (установить флаг cancel)
 * @param task AsyncTask*
 */
void async_cancel(AsyncTask* task);

/**
 * @brief Ожидать завершения асинхронной задачи с таймаутом (мс)
 * @param task AsyncTask*
 * @param timeout_ms Таймаут в миллисекундах
 * @return Result
 */
Result async_await_timeout(AsyncTask* task, int timeout_ms);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
// =====================
// C++ inline implementation (header-only)
// =====================
#include <memory>
#include <chrono>

namespace koala_async {
    inline void event_loop_worker(AsyncEventLoop* loop) {
        while (true) {
            AsyncTaskNode* node = nullptr;
            {
                std::unique_lock<std::mutex> lock(loop->queue_mtx);
                loop->queue_cv.wait(lock, [&]{ return !loop->queue.empty() || !loop->running; });
                if (!loop->running && loop->queue.empty()) break;
                if (!loop->queue.empty()) {
                    node = loop->queue.front();
                    loop->queue.pop();
                }
            }
            if (node) {
                if (node->task->cancel) {
                    // Отмена задачи
                    std::lock_guard<std::mutex> lock(node->task->mtx);
                    node->task->ready = true;
                } else {
                    Result res = node->func->func(node->params);
                    {
                        std::lock_guard<std::mutex> lock(node->task->mtx);
                        node->task->result = res;
                        node->task->ready = true;
                    }
                }
                node->task->cv.notify_all();
                delete node;
            }
        }
    }
}

inline AsyncEventLoop* async_event_loop_init(int num_workers = 1) {
    AsyncEventLoop* loop = new AsyncEventLoop;
    loop->running = true;
    loop->num_workers = num_workers;
    for (int i = 0; i < num_workers; ++i) {
        loop->workers.push_back(new std::thread(koala_async_detail::event_loop_worker, loop));
    }
    return loop;
}

inline void async_event_loop_stop(AsyncEventLoop* loop) {
    {
        std::lock_guard<std::mutex> lock(loop->queue_mtx);
        loop->running = false;
    }
    loop->queue_cv.notify_all();
    for (auto& worker : loop->workers) {
        if (worker && worker->joinable()) worker->join();
        delete worker;
    }
    loop->workers.clear();
    delete loop;
}

inline AsyncTask* async_run(AsyncEventLoop* loop, FunctionPipeline* func, FunctionParams* params) {
    AsyncTask* task = new AsyncTask;
    task->ready = false;
    task->cancel = false;
    AsyncTaskNode* node = new AsyncTaskNode;
    node->func = func;
    node->params = params;
    node->task = task;
    node->next = nullptr;
    {
        std::lock_guard<std::mutex> lock(loop->queue_mtx);
        loop->queue.push(node);
    }
    loop->queue_cv.notify_one();
    return task;
}

inline Result async_await(AsyncTask* task) {
    std::unique_lock<std::mutex> lock(task->mtx);
    task->cv.wait(lock, [&]{ return task->ready; });
    Result res = task->result;
    delete task;
    return res;
}

inline Result async_await_timeout(AsyncTask* task, int timeout_ms) {
    std::unique_lock<std::mutex> lock(task->mtx);
    if (task->cv.wait_for(lock, std::chrono::milliseconds(timeout_ms), [&]{ return task->ready; })) {
        Result res = task->result;
        delete task;
        return res;
    } else {
        // Таймаут: возвращаем Result с ошибкой
        Result res;
        res.is_ok = false;
        res.err = Generic<void*>((void*)"Timeout");
        delete task;
        return res;
    }
}

inline void async_cancel(AsyncTask* task) {
    std::lock_guard<std::mutex> lock(task->mtx);
    task->cancel = true;
}
#endif // __cplusplus

#endif // KOALA_ASYNC_H
