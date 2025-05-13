#ifndef KOALA_MATHEMATICA_H
#define KOALA_MATHEMATICA_H
#include "../types.h"

/**
 * @defgroup Mathematica Стандартная математика
 * @brief Модуль стандартной математики, задачи которого вычисляются на единственном процессоре.
 *
 * Этот модуль содержит структуру Vector и функции для работы с динамическими списками универсальных значений (Generic).
 *
 * @section vector_overview Обзор структуры Vector
 *
 * @code{.c}
 * typedef struct Vector {
 *     Generic* data;                ///< Указатель на данные (Generic)
 *     struct Vector* next;          ///< Следующий элемент списка
 *     void (*push_back)(struct Vector*, Generic* data); ///< Функция для добавления элемента
 * } Vector;
 * @endcode
 *
 * @section vector_fields Описание полей
 * - data: Указатель на объект Generic, хранящий данные.
 * - next: Указатель на следующий элемент вектора (реализован как односвязный список).
 * - push_back: Указатель на функцию для добавления элемента в конец вектора.
 *
 * @section vector_functions Основные функции
 * - Vector* init_vector(): Инициализация нового пустого вектора.
 * - void push(struct Vector* vec, Generic* data): Добавление элемента в конец вектора.
 * - void delete(struct Vector* vec): Удаление всех элементов вектора и освобождение памяти.
 * - void free_vector(struct Vector* vec): Синоним для delete.
 * - Vector* allocate_memory_for_vector(): Выделение памяти под новый вектор (обычно используется внутри init_vector).
 *
 * @section vector_usage Пример использования
 *
 * @code{.c}
 * #include "core/math/mathematica.h"
 * #include <stdio.h>
 *
 * int main() {
 *     Vector* vector = init_vector();
 *     if (vector == NULL) {
 *         // обработка ошибки
 *         return 1;
 *     }
 *     int value = 42;
 *     Generic g;
 *     GENERIC_SET(g, &value); // Сохраняем указатель на int
 *     g.type = GENERIC_TYPE_INT;
 *     vector->push_back(vector, &g);
 *
 *     // Извлечение значения из первого элемента вектора
 *     Generic* g_ptr = vector->data; // Предполагается, что data указывает на первый элемент Generic
 *     int* px = GENERIC_GET(int*, *g_ptr);
 *     printf("%d\n", *px); // 42
 *
 *     free_vector(vector);
 *     return 0;
 * }
 * @endcode
 */

/**
 * @defgroup VectorUsage Примеры использования векторов
 * @ingroup Mathematica
 *
 * @code{.c}
 * #include "core/math/mathematica.h"
 * #include <stdio.h>
 *
 * int main() {
 *     Vector* vector = init_vector();
 *     if (vector == NULL) {
 *         // обработка ошибки
 *         return 1;
 *     }
 *     int value = 42;
 *     Generic g;
 *     GENERIC_SET(g, &value); // Сохраняем указатель на int
 *     g.type = GENERIC_TYPE_INT;
 *     vector->push_back(vector, &g);
 *
 *     // Извлечение значения из первого элемента вектора
 *     Generic* g_ptr = vector->data; // Предполагается, что data указывает на первый элемент Generic
 *     int* px = GENERIC_GET(int*, *g_ptr);
 *     printf("%d\n", *px); // 42
 *
 *     free_vector(vector);
 *     return 0;
 * }
 * @endcode
 */

#ifndef __cplusplus
/**
 * @brief Структура Vector для C
 * @see push
 * @see delete
 * @see free_vector
 * @see allocate_memory_for_vector
 * @see init_vector
 */
typedef struct Vector {
    Generic* data; ///< Указатель на данные (Generic)
    struct Vector* next; ///< Следующий элемент списка
    void (*push_back)(struct Vector*, Generic* data); ///< Добавить элемент в конец
} Vector;

/**
 * @brief Добавить элемент в конец вектора.
 * @param vec Указатель на вектор
 * @param data Указатель на объект Generic
 */
void push(struct Vector* vec, Generic* data);

/**
 * @brief Удалить все элементы вектора и освободить память.
 * @param vec Указатель на вектор
 */
void delete(struct Vector* vec);

/**
 * @brief Освободить память, занятую вектором (синоним delete).
 * @param vec Указатель на вектор
 */
void free_vector(struct Vector* vec);

/**
 * @brief Выделить память под новый вектор (используется внутри init_vector).
 * @return Указатель на новый вектор или NULL при ошибке
 */
Vector* allocate_memory_for_vector();

/**
 * @brief Инициализировать новый пустой вектор.
 * @return Указатель на новый вектор или NULL при ошибке
 */
Vector* init_vector();
#endif // __cplusplus





#endif // KOALA_MATHEMATICA_H