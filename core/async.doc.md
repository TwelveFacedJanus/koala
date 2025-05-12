# Документация по core/async.h

## Описание

`core/async.h` реализует асинхронные примитивы (event loop, future, promise) для запуска функций из FunctionPipeline с поддержкой универсального результата (Result). Поддерживается как C, так и C++ (с использованием std::thread/std::mutex/std::condition_variable или pthread).

---

## Основные структуры

### AsyncTask
Дескриптор асинхронной задачи (future/promise). Позволяет ожидать результат выполнения функции.

- В C++:
  - `std::mutex mtx;`
  - `std::condition_variable cv;`
  - `bool ready;`
  - `Result result;`
- В C:
  - `pthread_mutex_t mtx;`
  - `pthread_cond_t cv;`
  - `int ready;`
  - `Result result;`

### AsyncTaskNode
Внутренняя структура для очереди задач event loop.
- `FunctionPipeline* func;`
- `FunctionParams* params;`
- `AsyncTask* task;`
- `AsyncTaskNode* next;`

### AsyncEventLoop
Event loop для асинхронного исполнения задач.
- В C++:
  - `std::thread* worker;`
  - `std::mutex queue_mtx;`
  - `std::condition_variable queue_cv;`
  - `std::queue<AsyncTaskNode*> queue;`
  - `bool running;`
- В C:
  - `pthread_t worker;`
  - `pthread_mutex_t queue_mtx;`
  - `pthread_cond_t queue_cv;`
  - `AsyncTaskNode* queue_head, *queue_tail;`
  - `int running;`

---

## Основные функции

### AsyncEventLoop* async_event_loop_init();
Создаёт и запускает event loop. Возвращает указатель на структуру event loop.

### void async_event_loop_stop(AsyncEventLoop* loop);
Останавливает event loop, освобождает ресурсы.

### AsyncTask* async_run(AsyncEventLoop* loop, FunctionPipeline* func, FunctionParams* params);
Добавляет задачу в event loop для асинхронного исполнения. Возвращает дескриптор задачи (future).

### Result async_await(AsyncTask* task);
Блокирует вызывающий поток до завершения задачи, возвращает результат (Result). Освобождает ресурсы задачи.

---

## Интеграция с FunctionPipeline и Result
- Любая функция, совместимая с FunctionPipeline (принимает `FunctionParams*`, возвращает `Result`), может быть запущена асинхронно через event loop.
- Результат выполнения всегда возвращается в виде структуры Result (см. core/types.h).
- Аргументы для функции передаются через структуру FunctionParams (argc, argv).

---

## Новые возможности (v2)

### Пул потоков
- Event loop теперь поддерживает несколько воркеров (потоков).
- При инициализации можно указать число воркеров: `async_event_loop_init(num_workers)` (C/C++).
- Все задачи из очереди распределяются между воркерами.

### Отмена задач
- В структуре AsyncTask появился флаг cancel.
- Для отмены задачи вызовите `async_cancel(task)`.
- Если задача не была начата, она не будет выполнена; если уже выполняется — отмена не гарантируется (зависит от реализации).

### Таймауты
- Для ожидания результата с таймаутом используйте `async_await_timeout(task, timeout_ms)`.
- Если задача не завершилась за указанное время, возвращается Result с ошибкой (например, err = "Timeout").

---

## Примеры использования

### Простой пример (C)
```c
#include "types.h"
#include "async.h"
#include <stdio.h>

Result hello_func(FunctionParams* params) {
    Result res;
    res.is_ok = 1;
    GENERIC_SET(res.ok, "Hello from async!");
    return res;
}

int main() {
    AsyncEventLoop* loop = async_event_loop_init();
    FunctionPipeline fp = { hello_func, NULL };
    FunctionParams params = { 0, NULL };
    AsyncTask* task = async_run(loop, &fp, &params);
    Result r = async_await(task);
    if (r.is_ok) printf("%s\n", (char*)GENERIC_GET(char*, r.ok));
    async_event_loop_stop(loop);
    return 0;
}
```

### Пример с аргументами (C)
```c
#include "types.h"
#include "async.h"
#include <stdio.h>

Result sum_func(FunctionParams* params) {
    Result res;
    if (params->argc < 2) {
        res.is_ok = 0;
        GENERIC_SET(res.err, "Need 2 args");
        return res;
    }
    int a = *(int*)GENERIC_GET(int*, params->argv[0]);
    int b = *(int*)GENERIC_GET(int*, params->argv[1]);
    int* sum = malloc(sizeof(int));
    *sum = a + b;
    res.is_ok = 1;
    GENERIC_SET(res.ok, sum);
    return res;
}

int main() {
    AsyncEventLoop* loop = async_event_loop_init();
    int x = 2, y = 3;
    Generic argv[2];
    GENERIC_SET(argv[0], &x);
    GENERIC_SET(argv[1], &y);
    FunctionParams params = { 2, argv };
    FunctionPipeline fp = { sum_func, NULL };
    AsyncTask* task = async_run(loop, &fp, &params);
    Result r = async_await(task);
    if (r.is_ok) {
        int* sum = GENERIC_GET(int*, r.ok);
        printf("Sum: %d\n", *sum);
        free(sum);
    } else {
        printf("Error: %s\n", (char*)GENERIC_GET(char*, r.err));
    }
    async_event_loop_stop(loop);
    return 0;
}
```

### Пример с несколькими задачами (C++)
```cpp
#include "types.h"
#include "async.h"
#include <iostream>
#include <vector>

Result hello_func(FunctionParams* params) {
    Result res;
    res.is_ok = true;
    res.ok = Generic<const char*>("Hello from async!");
    return res;
}

int main() {
    AsyncEventLoop* loop = async_event_loop_init();
    FunctionPipeline fp = { hello_func, nullptr };
    std::vector<AsyncTask*> tasks;
    for (int i = 0; i < 5; ++i) {
        FunctionParams params = { 0, nullptr };
        tasks.push_back(async_run(loop, &fp, &params));
    }
    for (auto task : tasks) {
        Result r = async_await(task);
        if (r.is_ok) std::cout << static_cast<const char*>(r.ok.value) << std::endl;
    }
    async_event_loop_stop(loop);
    return 0;
}
```

### Пример обработки ошибок (C++)
```cpp
#include "types.h"
#include "async.h"
#include <iostream>

Result fail_func(FunctionParams* params) {
    Result res;
    res.is_ok = false;
    res.err = Generic<const char*>("Something went wrong");
    return res;
}

int main() {
    AsyncEventLoop* loop = async_event_loop_init();
    FunctionPipeline fp = { fail_func, nullptr };
    FunctionParams params = { 0, nullptr };
    AsyncTask* task = async_run(loop, &fp, &params);
    Result r = async_await(task);
    if (!r.is_ok) std::cout << "Error: " << static_cast<const char*>(r.err.value) << std::endl;
    async_event_loop_stop(loop);
    return 0;
}
```

### Пул потоков (C++)
```cpp
AsyncEventLoop* loop = async_event_loop_init(4); // 4 воркера
// ... запуск задач ...
async_event_loop_stop(loop);
```

### Ожидание с таймаутом (C++)
```cpp
AsyncTask* task = async_run(loop, &fp, &params);
Result r = async_await_timeout(task, 1000); // ждать максимум 1 секунду
if (!r.is_ok) std::cout << "Timeout or error: " << static_cast<const char*>(r.err.value) << std::endl;
```

### Отмена задачи (C++)
```cpp
AsyncTask* task = async_run(loop, &fp, &params);
async_cancel(task); // попытка отменить задачу
Result r = async_await(task);
if (!r.is_ok) std::cout << "Cancelled or error: " << static_cast<const char*>(r.err.value) << std::endl;
```

---

## Потокобезопасность
- Все операции с очередью задач и состоянием event loop защищены mutex'ами.
- async_await потокобезопасен для каждого отдельного AsyncTask.

## Ограничения и замечания
- В C++ реализация полностью header-only. В C требуется реализация функций в .c-файле.
- Освобождение памяти для аргументов и результатов (если выделялись динамически) ложится на пользователя.
- Event loop однопоточный (один worker). Для расширения до пула потоков — модифицируйте AsyncEventLoop.
- После async_await дескриптор задачи освобождается автоматически (в C++). В C — рекомендуется явно вызывать free, если реализация выделяет память.

## Расширяемость
- Можно реализовать несколько event loop'ов для разных задач.
- Можно расширить FunctionParams для передачи дополнительных данных.
- Для поддержки отмены задач — добавить флаг отмены в AsyncTask и проверку в worker.

---

## См. также
- `core/types.h` — описание FunctionPipeline, Result, Generic.
- Примеры в Doxygen-блоках внутри async.h.

## Иллюстрированные схемы

### Архитектура Event Loop

```
+-------------------+
| AsyncEventLoop    |
|-------------------|
| worker (thread)   |
| queue (tasks)     |
| running (flag)    |
+-------------------+
         |
         v
   [Worker Thread]
         |
         v
+-------------------+
| AsyncTaskNode     |
|-------------------|
| func, params, ... |
+-------------------+
```

### Очередь задач (C++)
```
+-------------------+    +-------------------+    +-------------------+
| AsyncTaskNode     |--->| AsyncTaskNode     |--->| AsyncTaskNode     |---> NULL
+-------------------+    +-------------------+    +-------------------+
```

### Взаимодействие async_run и async_await

```
[main thread]
    |
    | async_run(loop, &fp, &params)
    v
[AsyncEventLoop] (кладёт задачу в очередь)
    |
    v
[Worker Thread] --(выполняет)-> [FunctionPipeline.func]
    |
    v
[AsyncTask] (готов)
    |
    v
[main thread] -- async_await --> [Result]
```

### Структура AsyncTask
```
+-----------------------+
|      AsyncTask        |
|-----------------------|
| mtx/cv (sync)         |
| ready (bool/int)      |
| result (Result)       |
+-----------------------+
```

### Поток данных через Event Loop
```
[FunctionParams]
     |
     v
[FunctionPipeline.func] --(Result)--> [AsyncTask]
     |                                    ^
     v                                    |
[FunctionPipeline.next] ...         [async_await]
```

## Пояснения по новым полям структур

- `AsyncEventLoop::num_workers` — количество потоков-воркеров.
- `AsyncEventLoop::workers` — массив/вектор потоков.
- `AsyncTask::cancel` — флаг отмены задачи. 