#include "milena_queue.h"
#include <cstdio>

int main() {
    Queue* qu = queue_create();

    Task t1 = { "Task1", "echo Hello", 1, TASK_PENDING };
    Task t2 = { "Task2", "ls -l", 2, TASK_PENDING };
    Task t3 = { "Task3", "date", 3, TASK_PENDING };

    queue_append(qu, &t1);
    queue_append(qu, &t2);
    queue_append(qu, &t3);

    queue_remove_by_index(qu, 1);

    for (int i = 0; i < queue_size(qu); ++i) {
        const Task* t = queue_get(qu, i);
        printf("Task: %s, Command: %s, Priority: %d, Status: %d\n",
               t->name, t->command, t->priority, t->status);
    }

    queue_destroy(qu);
    return 0;
}