/**
 * @file milena_queue.h
 * @brief Очередь задач для распределённых вычислений (C/C++ API).
 *
 * Реализует динамическую очередь задач с возможностью добавления, удаления и доступа по индексу.
 * Поддерживает совместимость с C и C++.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum TaskStatus
 * @brief Статус задачи в очереди.
 */
typedef enum TaskStatus {
    TASK_DONE,    /**< Задача выполнена */
    TASK_PENDING, /**< Задача ожидает выполнения */
    TASK_ERROR    /**< При выполнении задачи произошла ошибка */
} TaskStatus;

/**
 * @struct Task
 * @brief Описание задачи для очереди.
 *
 * Хранит имя задачи, команду для запуска, приоритет и статус.
 */
typedef struct Task {
    const char* name;      /**< Имя задачи */
    const char* command;   /**< Команда для запуска */
    int priority;          /**< Приоритет задачи (чем выше, тем важнее) */
    TaskStatus status;     /**< Статус задачи */
} Task;

/**
 * @struct Queue
 * @brief Очередь задач (реализация скрыта в C++, opaque pointer для C).
 */
typedef struct Queue Queue;

/**
 * @brief Создать новую очередь задач.
 * @return Указатель на очередь (Queue*) или NULL при ошибке.
 *
 * @code{.c}
 * Queue* q = queue_create();
 * @endcode
 */
Queue* queue_create();

/**
 * @brief Удалить очередь и освободить всю связанную с ней память.
 * @param q Указатель на очередь
 *
 * @code{.c}
 * queue_destroy(q);
 * @endcode
 */
void queue_destroy(Queue* q);

/**
 * @brief Добавить задачу в конец очереди.
 * @param q Указатель на очередь
 * @param task Указатель на структуру Task
 *
 * @code{.c}
 * Task t = {"Task1", "echo Hello", 1, TASK_PENDING};
 * queue_append(q, &t);
 * @endcode
 */
void queue_append(Queue* q, const Task* task);

/**
 * @brief Удалить задачу по индексу из очереди.
 * @param q Указатель на очередь
 * @param index Индекс задачи (начиная с 0)
 * @return 1 если задача удалена, 0 если индекс некорректен
 *
 * @code{.c}
 * queue_remove_by_index(q, 2);
 * @endcode
 */
int queue_remove_by_index(Queue* q, int index);

/**
 * @brief Получить количество задач в очереди.
 * @param q Указатель на очередь
 * @return Количество задач (>=0)
 *
 * @code{.c}
 * int n = queue_size(q);
 * @endcode
 */
int queue_size(const Queue* q);

/**
 * @brief Получить задачу по индексу.
 * @param q Указатель на очередь
 * @param index Индекс задачи (начиная с 0)
 * @return Указатель на задачу (Task*) или NULL, если индекс некорректен
 *
 * @code{.c}
 * const Task* t = queue_get(q, 0);
 * @endcode
 */
const Task* queue_get(const Queue* q, int index);

#ifdef __cplusplus
}
#endif