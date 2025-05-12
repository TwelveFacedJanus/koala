# Документация по core/types.h

## Описание

`core/types.h` содержит основные типы и структуры для кроссплатформенной работы с числами, универсальными значениями, результатами вычислений, параметрами функций и пайплайнами функций. Все типы совместимы с C и C++ и предназначены для интеграции с асинхронными примитивами (`async.h`).

---

## Основные типы

### Целочисленные алиасы
- `u8`, `u16`, `u32`, `u64` — стандартные беззнаковые целые.
- `i8`, `i16`, `i32`, `i64` — стандартные знаковые целые.
- `ufast8`, ... — быстрые целые (оптимизированы под скорость).
- `uleast8`, ... — наименьшие целые (минимальный размер).
- `uptr`, `iptr` — для хранения указателей.
- `usize`, `isize` — для размеров и разницы указателей.

### Generic
Универсальный контейнер для хранения значения любого типа.
- В C++: шаблон `Generic<T>`, поле `T value`.
- В C: структура с полем `void* value`.
- Макросы для C: `GENERIC_SET(g, v)`, `GENERIC_GET(type, g)`.

#### Пример (C)
```c
Generic g;
int x = 42;
GENERIC_SET(g, &x);
int* px = GENERIC_GET(int*, g);
printf("%d\n", *px); // 42
```

#### Пример (C++)
```cpp
Generic<int> g(42);
std::cout << g.value << std::endl; // 42
Generic<std::string> gs(std::string("hello"));
std::cout << gs.value << std::endl; // hello
```

### Result
Аналог Rust Result: хранит либо успешное значение, либо ошибку, оба — в виде Generic.
- В C++: структура с `bool is_ok`, `Generic<void*> ok`, `Generic<void*> err`.
- В C: структура с `int is_ok`, `Generic ok`, `Generic err`.

#### Пример (C)
```c
Result res;
int x = 42;
res.is_ok = 1;
GENERIC_SET(res.ok, &x);
if (res.is_ok) {
    int* px = GENERIC_GET(int*, res.ok);
    printf("%d\n", *px);
} else {
    const char* err = GENERIC_GET(const char*, res.err);
    printf("Ошибка: %s\n", err);
}
```

#### Пример (C++)
```cpp
Result res;
int x = 42;
res.is_ok = true;
res.ok = Generic<int*>(&x);
if (res.is_ok) {
    int* px = static_cast<int*>(res.ok.value);
    std::cout << *px << std::endl;
} else {
    const char* err = static_cast<const char*>(res.err.value);
    std::cout << "Ошибка: " << err << std::endl;
}
```

### FunctionParams
Параметры для функций в FunctionPipeline.
- В C++: `int argc`, `Generic<void*>* argv`.
- В C: `int argc`, `Generic* argv`.
- Используется для передачи аргументов в пайплайн-функции.

#### Пример (C)
```c
int x = 1, y = 2;
Generic argv[2];
GENERIC_SET(argv[0], &x);
GENERIC_SET(argv[1], &y);
FunctionParams params = { 2, argv };
```

#### Пример (C++)
```cpp
int x = 1, y = 2;
Generic<void*> argv[2] = { Generic<void*>(&x), Generic<void*>(&y) };
FunctionParams params = { 2, argv };
```

### FunctionPipeline
Элемент цепочки функций (pipeline).
- Поля: `Result (*func)(FunctionParams*)`, `FunctionPipeline* next`.
- Позволяет строить цепочки обработки данных.

#### Пример (C)
```c
Result my_func(FunctionParams* params) { /* ... */ }
FunctionPipeline fp = { my_func, NULL };
```

#### Пример (C++)
```cpp
Result my_func(FunctionParams* params) { /* ... */ }
FunctionPipeline fp = { my_func, nullptr };
```

---

## Интеграция с async.h
- Все типы совместимы с асинхронными примитивами: можно передавать FunctionPipeline, FunctionParams, Result в async_run, async_await.
- Generic позволяет безопасно передавать любые типы данных между потоками.

---

## Рекомендации по памяти и безопасности
- Если Generic хранит указатель на динамически выделенную память, освобождайте её вручную после использования.
- Для Result: если ok/err содержит динамический ресурс, освобождайте после обработки.
- FunctionParams::argv может указывать на массив Generic, который также может требовать освобождения.

---

## Расширяемость
- Можно добавлять новые поля в FunctionParams для передачи контекста.
- Можно строить сложные пайплайны, комбинируя FunctionPipeline через поле next.
- Для поддержки дополнительных сценариев (например, отмена, таймауты) расширяйте Result и FunctionPipeline.

---

## Пример комплексного использования с async.h (C)
```c
#include "types.h"
#include "async.h"
#include <stdio.h>

Result sum_func(FunctionParams* params) {
    Result res;
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
    int x = 10, y = 20;
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
    }
    async_event_loop_stop(loop);
    return 0;
}
```

---

## См. также
- `core/async.h` — асинхронные примитивы и event loop.
- Doxygen-блоки с примерами внутри types.h.

---

## FAQ

**Q: Можно ли хранить в Generic не только указатели, но и значения?**
A: В C — только указатели (void*). В C++ можно использовать Generic<T> для любых типов, но для совместимости с FunctionParams/Result рекомендуется использовать указатели.

**Q: Как правильно освобождать память, если Result содержит динамический ресурс?**
A: После обработки результата (например, после async_await) проверьте, что ok/err содержит указатель на динамический ресурс, и вызовите free или delete вручную.

**Q: Можно ли передавать FunctionParams между потоками?**
A: Да, если все данные внутри argv корректно защищены или не используются конкурентно. Для передачи больших структур используйте указатели.

**Q: Как построить цепочку FunctionPipeline?**
A: Каждый элемент содержит указатель на следующий. Можно динамически строить цепочки любой длины.

**Q: Как интегрировать с async.h?**
A: Просто передайте FunctionPipeline и FunctionParams в async_run. Результат получите через async_await.

---

## Иллюстрированные схемы

### Generic (C)

```
+-------------------+
|   Generic         |
|-------------------|
| void* value       |
+-------------------+
```

### Generic<T> (C++)
```
+-------------------+
|   Generic<T>      |
|-------------------|
| T value           |
+-------------------+
```

### Result
```
+-------------------+
|   Result          |
|-------------------|
| is_ok (bool/int)  |
| ok:   Generic     |
| err:  Generic     |
+-------------------+
```

### FunctionParams
```
+-------------------+
| FunctionParams    |
|-------------------|
| int argc          |
| Generic* argv     |
+-------------------+
```

### FunctionPipeline (цепочка)

```
+-------------------+      +-------------------+
| FunctionPipeline  | ---> | FunctionPipeline  | ---> ...
|-------------------|      |-------------------|
| func              |      | func              |
| next ------------+      | next ------------+
+-------------------+      +-------------------+
```

### Поток данных через FunctionPipeline и async

```
[FunctionParams]
     |
     v
[FunctionPipeline.func] --(Result)--> [async_run]
     |                                    |
     v                                    v
[FunctionPipeline.next] ...         [AsyncEventLoop]
                                         |
                                         v
                                    [async_await]
                                         |
                                         v
                                      [Result]
```

---

## Типичные ошибки и их решения

**Ошибка:** Использование Generic для хранения значения, а не указателя (в C).
- **Решение:** Всегда используйте указатель: `GENERIC_SET(g, &x);` и `GENERIC_GET(int*, g);`

**Ошибка:** Утечка памяти при использовании динамических данных в Result/Generic.
- **Решение:** После обработки результата обязательно вызывайте free/delete для выделенных данных.

**Ошибка:** Неправильное приведение типа при извлечении из Generic.
- **Решение:** Убедитесь, что тип в GENERIC_GET совпадает с тем, что был сохранён.

**Ошибка:** Забыли инициализировать поле next в FunctionPipeline.
- **Решение:** Всегда явно указывайте next (NULL/nullptr, если это конец цепочки).

**Ошибка:** Использование FunctionParams после освобождения argv.
- **Решение:** Убедитесь, что argv живёт до завершения работы функции/пайплайна. 