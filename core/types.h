#ifndef KOALA_TYPES_H
#define KOALA_TYPES_H

#ifdef __cplusplus
    #include <cstdint>
    #include <cstddef>  // Для size_t, ptrdiff_t
#else
    #include <stdint.h>
    #include <stddef.h> // Для size_t, ptrdiff_t
#endif

// Максимальные целые типы
typedef intmax_t    MAX_INT;
typedef uintmax_t   MAX_UINT;

// Стандартные беззнаковые целые
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

// Стандартные знаковые целые
typedef int8_t      	i8;
typedef int16_t     	i16;
typedef int32_t     	i32;
typedef int64_t     	i64;

// Быстрые типы (оптимизированы под скорость, а не размер)
typedef uint_fast8_t    ufast8;
typedef uint_fast16_t   ufast16;
typedef uint_fast32_t   ufast32;
typedef uint_fast64_t   ufast64;

typedef int_fast8_t     ifast8;
typedef int_fast16_t    ifast16;
typedef int_fast32_t    ifast32;
typedef int_fast64_t    ifast64;

// Наименьшие типы (занимают минимум места)
typedef uint_least8_t   uleast8;
typedef uint_least16_t  uleast16;
typedef uint_least32_t  uleast32;
typedef uint_least64_t  uleast64;

typedef int_least8_t    ileast8;
typedef int_least16_t   ileast16;
typedef int_least32_t   ileast32;
typedef int_least64_t   ileast64;

// Типы для указателей (беззнаковый/знаковый)
typedef uintptr_t       uptr;
typedef intptr_t        iptr;

// Типы для размеров объектов и разницы указателей
typedef size_t          usize;   // Беззнаковый размер (аналог sizeof)
typedef ptrdiff_t       isize;   // Знаковый размер (разница указателей)

// Специальные типы
#ifdef __cplusplus
    using nullptr_t = std::nullptr_t;  // Тип nullptr в C++
#endif

// =====================
// Generic types block
// =====================
/**
 * @defgroup GenericTypes Generic Types
 * @brief Универсальные типы для хранения любых значений.
 *
 * Примеры использования:
 *
 * @code{.c}
 * // C: Использование структуры Generic
 * #include "types.h"
 * Generic g;
 * int x = 42;
 * GENERIC_SET(g, &x); // Сохраняем указатель на int
 * int* px = GENERIC_GET(int*, g); // Получаем указатель обратно
 * printf("%d\n", *px); // 42
 * @endcode
 *
 * @code{.cpp}
 * // C++: Использование шаблона Generic<T>
 * #include "types.h"
 * Generic<int> g(42);
 * std::cout << g.value << std::endl; // 42
 *
 * // Можно использовать с любым типом
 * Generic<std::string> gs(std::string("hello"));
 * std::cout << gs.value << std::endl; // hello
 * @endcode
 */
#ifdef __cplusplus
    // C++: шаблон Generic<T>
    template<typename T>
    struct Generic {
        T value;
        Generic() = default;
        Generic(const T& v) : value(v) {}
    };
#else
    // C: структура Generic с void*
    typedef struct Generic {
        void* value;
    } Generic;
    // Макрос для приведения типов
    #define GENERIC_SET(g, v)   ((g).value = (void*)(v))
    #define GENERIC_GET(type, g) ((type)((g).value))
#endif

// =====================
// Result type block (Rust-like, simplified)
// =====================
/**
 * @defgroup ResultType Result Type
 * @brief Упрощённый аналог типа Result из Rust для C и C++.
 *
 * Хранит либо Generic-значение (ok), либо Generic-ошибку (err).
 *
 * Примеры использования:
 *
 * @code{.c}
 * // C: Использование структуры Result
 * #include "types.h"
 * Result res;
 * int x = 42;
 * res.is_ok = 1;
 * GENERIC_SET(res.ok, &x); // Сохраняем указатель на int
 * // ...
 * if (res.is_ok) {
 *     int* px = GENERIC_GET(int*, res.ok);
 *     printf("%d\n", *px);
 * } else {
 *     const char* err = GENERIC_GET(const char*, res.err);
 *     printf("Ошибка: %s\n", err);
 * }
 * @endcode
 *
 * @code{.cpp}
 * // C++: Использование структуры Result
 * #include "types.h"
 * Result res;
 * int x = 42;
 * res.is_ok = true;
 * res.ok = Generic<int*>(&x);
 * // ...
 * if (res.is_ok) {
 *     int* px = static_cast<int*>(res.ok.value);
 *     std::cout << *px << std::endl;
 * } else {
 *     const char* err = static_cast<const char*>(res.err.value);
 *     std::cout << "Ошибка: " << err << std::endl;
 * }
 * @endcode
 */
#ifdef __cplusplus
    struct Result {
        bool is_ok;
        Generic<void*> ok;
        Generic<void*> err;
        Result() : is_ok(false), ok(), err() {}
        Result(const Generic<void*>& v) : is_ok(true), ok(v), err() {}
        Result(const Generic<void*>& v, const Generic<void*>& e) : is_ok(false), ok(v), err(e) {}
    };
#else
    typedef struct Result {
        int is_ok; // 1 = ok, 0 = err
        Generic ok;
        Generic err;
    } Result;
#endif

// =====================
// Function pipeline types
// =====================
#ifdef __cplusplus
/**
 * @brief Структура параметров для функций в FunctionPipeline (C++).
 * argc: количество аргументов
 * argv: массив аргументов типа Generic<void*>
 */
typedef struct FunctionParams {
    int argc;                 /**< Количество аргументов */
    Generic<void*>* argv;     /**< Массив аргументов (Generic<void*>) */
} FunctionParams;
#else
/**
 * @brief Структура параметров для функций в FunctionPipeline (C).
 * argc: количество аргументов
 * argv: массив аргументов типа Generic
 */
typedef struct FunctionParams {
    int argc;           /**< Количество аргументов */
    Generic* argv;      /**< Массив аргументов (Generic) */
} FunctionParams;
#endif

/**
 * @brief Элемент цепочки функций (pipeline).
 *
 * func: функция, принимающая указатель на FunctionParams и возвращающая Result.
 * next: следующий элемент цепочки (или NULL).
 */
typedef struct FunctionPipeline {
    Result (*func)(FunctionParams* params);
    struct FunctionPipeline* next;
} FunctionPipeline;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Освобождает ресурсы, связанные с Result (C).
 * Если ok/err содержит динамический ресурс, освобождает его.
 */
void result_free(Result* r);

/**
 * @brief Освобождает ресурсы, связанные с FunctionParams (C).
 * Освобождает argv, если он был выделен динамически.
 */
void function_params_free(FunctionParams* p);

/**
 * @brief Освобождает всю цепочку FunctionPipeline (C).
 * Рекурсивно освобождает next.
 */
void function_pipeline_free(FunctionPipeline* p);

#ifdef __cplusplus
}

// RAII-обертки для C++
#include <memory>

/**
 * @brief RAII-обертка для автоматического освобождения Result
 */
struct ResultGuard {
    Result* r;
    explicit ResultGuard(Result* r_) : r(r_) {}
    ~ResultGuard() { result_free(r); }
};

/**
 * @brief RAII-обертка для FunctionParams
 */
struct FunctionParamsGuard {
    FunctionParams* p;
    explicit FunctionParamsGuard(FunctionParams* p_) : p(p_) {}
    ~FunctionParamsGuard() { function_params_free(p); }
};

/**
 * @brief RAII-обертка для FunctionPipeline
 */
struct FunctionPipelineGuard {
    FunctionPipeline* fp;
    explicit FunctionPipelineGuard(FunctionPipeline* fp_) : fp(fp_) {}
    ~FunctionPipelineGuard() { function_pipeline_free(fp); }
};
#endif

#ifndef __cplusplus
/**
 * @brief Перечисление поддерживаемых типов для Generic (C)
 */
typedef enum {
    GENERIC_TYPE_UNKNOWN = 0,
    GENERIC_TYPE_INT,
    GENERIC_TYPE_FLOAT,
    GENERIC_TYPE_PTR,
    GENERIC_TYPE_STR,
    GENERIC_TYPE_CUSTOM
} GenericType;

/**
 * @brief Generic с типизацией (C)
 */
typedef struct Generic {
    void* value;
    GenericType type;
} Generic;

/**
 * @brief Установить значение и тип в Generic
 */
#define GENERIC_SET_TYPED(g, v, t) do { (g).value = (void*)(v); (g).type = (t); } while(0)

/**
 * @brief Получить значение из Generic с проверкой типа
 */
#define GENERIC_GET_TYPED(type_, g, expected_type) \
    (((g).type == (expected_type)) ? (type_)((g).value) : NULL)

// Для обратной совместимости
#define GENERIC_SET(g, v)   GENERIC_SET_TYPED(g, v, GENERIC_TYPE_PTR)
#define GENERIC_GET(type_, g) GENERIC_GET_TYPED(type_, g, GENERIC_TYPE_PTR)
#endif

/**
 * @brief Создать новый элемент FunctionPipeline (C)
 * @param f - функция
 * @return указатель на FunctionPipeline
 */
#define FUNCTION_PIPELINE_NEW(f) \
    ({ \
        FunctionPipeline* _fp = (FunctionPipeline*)malloc(sizeof(FunctionPipeline)); \
        _fp->func = (f); \
        _fp->next = NULL; \
        _fp; \
    })

/**
 * @brief Добавить элемент в конец цепочки FunctionPipeline (C)
 * @param head - начало цепочки
 * @param node - новый элемент
 */
#define FUNCTION_PIPELINE_APPEND(head, node) do { \
    FunctionPipeline* _cur = (head); \
    if (!_cur) { (head) = (node); } \
    else { while (_cur->next) _cur = _cur->next; _cur->next = (node); } \
} while(0)

/**
 * @brief Проверить корректность цепочки FunctionPipeline (нет циклов, все func != NULL)
 * @param head - начало цепочки
 * @return 1 если корректна, 0 если есть ошибка
 */
int function_pipeline_validate(FunctionPipeline* head);

#endif // KOALA_TYPES_H
