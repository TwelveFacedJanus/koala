#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "ant_logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include "../../../core/async.h"
#include "../../../core/types.h"
#include "../../../core/math/mathematica.h"

/// Уровни логирования
typedef enum {
    ANT_LOG_DEBUG,
    ANT_LOG_INFO,
    ANT_LOG_WARNING,
    ANT_LOG_ERROR,
    ANT_LOG_FATAL
} ant_log_level_t;

/// Цветной вывод (по умолчанию включён для терминала)
void ant_log_enable_colors(bool enable);

/// Асинхронный режим (по умолчанию выключен)
void ant_log_enable_async(bool enable);

/// Инициализация логгера (опционально: путь к файлу, callback)
void ant_log_init(const char* file_path);

/// Завершение работы логгера (закрытие файлов и т.д.)
void ant_log_shutdown();

/// Установить минимальный уровень логирования
void ant_log_set_level(ant_log_level_t level);

/// Установить пользовательский callback для логирования
typedef void (*ant_log_callback_t)(ant_log_level_t level, const char* module, const char* message);
void ant_log_set_callback(ant_log_callback_t cb);

/// Основная функция логирования (не используйте напрямую, используйте макросы)
void ant_log_write(ant_log_level_t level, const char* module, const char* file, int line, const char* func, const char* fmt, ...);

/// Макросы для удобного логирования с указанием модуля
#define ANT_LOG_DEBUG_M(module, fmt, ...)   ant_log_write(ANT_LOG_DEBUG,   module, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define ANT_LOG_INFO_M(module, fmt, ...)    ant_log_write(ANT_LOG_INFO,    module, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define ANT_LOG_WARNING_M(module, fmt, ...) ant_log_write(ANT_LOG_WARNING, module, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define ANT_LOG_ERROR_M(module, fmt, ...)   ant_log_write(ANT_LOG_ERROR,   module, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define ANT_LOG_FATAL_M(module, fmt, ...)   ant_log_write(ANT_LOG_FATAL,   module, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

/// Макросы без указания модуля (используется "default")
#define ANT_LOG_DEBUG(fmt, ...)   ANT_LOG_DEBUG_M("default", fmt, ##__VA_ARGS__)
#define ANT_LOG_INFO(fmt, ...)    ANT_LOG_INFO_M("default", fmt, ##__VA_ARGS__)
#define ANT_LOG_WARNING(fmt, ...) ANT_LOG_WARNING_M("default", fmt, ##__VA_ARGS__)
#define ANT_LOG_ERROR(fmt, ...)   ANT_LOG_ERROR_M("default", fmt, ##__VA_ARGS__)
#define ANT_LOG_FATAL(fmt, ...)   ANT_LOG_FATAL_M("default", fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif 