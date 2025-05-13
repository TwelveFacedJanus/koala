// milena_queue.cpp
#include "milena_queue.h"
#include <string>
#include <vector>
#include <memory>

struct TaskCpp {
    std::string name;
    std::string command;
    int priority;
    TaskStatus status;

    TaskCpp(const Task& t)
        : name(t.name ? t.name : ""), command(t.command ? t.command : ""), priority(t.priority), status(t.status) {}
    Task to_c() const {
        Task t;
        t.name = name.c_str();
        t.command = command.c_str();
        t.priority = priority;
        t.status = status;
        return t;
    }
};

struct Queue {
    std::vector<TaskCpp> tasks;
};

Queue* queue_create() {
    return new Queue();
}

void queue_destroy(Queue* q) {
    delete q;
}

void queue_append(Queue* q, const Task* task) {
    if (q && task) {
        q->tasks.emplace_back(*task);
    }
}

int queue_remove_by_index(Queue* q, int index) {
    if (!q || index < 0 || index >= (int)q->tasks.size()) return 0;
    q->tasks.erase(q->tasks.begin() + index);
    return 1;
}

int queue_size(const Queue* q) {
    return q ? (int)q->tasks.size() : 0;
}

const Task* queue_get(const Queue* q, int index) {
    if (!q || index < 0 || index >= (int)q->tasks.size()) return nullptr;
    // ВНИМАНИЕ: возвращаем временный Task, который живёт до конца функции!
    // Для реального API лучше возвращать копию через out-параметр.
    static thread_local Task temp;
    temp = q->tasks[index].to_c();
    return &temp;
}