#include "ant_logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include "../../../core/async.h"

#define ANT_LOG_MAX_MSG 1024
#define ANT_LOG_MAX_MODULE 64

/**
 * @defgroup AntLogger Логгер AntLogger
 * @brief Многофункциональный потокобезопасный логгер с поддержкой асинхронности, модулей и цветного вывода.
 * @{
 */

/**
 * @brief Файл для логирования (NULL = stderr)
 */
static FILE* log_file = NULL;
/**
 * @brief Минимальный уровень логирования
 */
static ant_log_level_t min_level = ANT_LOG_DEBUG;
/**
 * @brief Пользовательский callback для логирования
 */
static ant_log_callback_t user_callback = NULL;
/**
 * @brief Включён ли цветной вывод
 */
static int use_colors = 1;
/**
 * @brief Включён ли асинхронный режим
 */
static int use_async = 0;
/**
 * @brief Event loop для асинхронного логирования
 */
static AsyncEventLoop* log_loop = NULL;
/**
 * @brief Мьютекс для потокобезопасности
 */
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

static const char* level_colors[] = {
    "\033[0;37m", // DEBUG - white
    "\033[0;32m", // INFO - green
    "\033[1;33m", // WARNING - yellow
    "\033[0;31m", // ERROR - red
    "\033[1;41m"  // FATAL - red bg
};
static const char* level_names[] = {
    "DEBUG", "INFO", "WARNING", "ERROR", "FATAL"
};

/**
 * @brief Включить или отключить цветной вывод.
 * @param enable 1 - включить, 0 - отключить
 */
void ant_log_enable_colors(bool enable) { use_colors = enable ? 1 : 0; }

/**
 * @brief Включить или отключить асинхронный режим логирования.
 * @param enable 1 - включить, 0 - отключить
 */
void ant_log_enable_async(bool enable) {
    if (enable && !use_async) {
        log_loop = async_event_loop_init();
        if (!log_loop) {
            fprintf(stderr, "[ant_logger] ERROR: Failed to initialize async event loop!\n");
            return;
        }
        use_async = 1;
    } else if (!enable && use_async) {
        async_event_loop_stop(log_loop);
        log_loop = NULL;
        use_async = 0;
    }
}

/**
 * @brief Инициализация логгера.
 * @param file_path Путь к файлу для логирования (NULL = stderr)
 */
void ant_log_init(const char* file_path) {
    if (file_path) {
        log_file = fopen(file_path, "a");
        if (!log_file) {
            fprintf(stderr, "[ant_logger] ERROR: Failed to open log file '%s'! Logging to stderr.\n", file_path);
            log_file = NULL;
        }
    }
}

/**
 * @brief Завершение работы логгера, освобождение ресурсов.
 */
void ant_log_shutdown() {
    if (log_file) fclose(log_file);
    log_file = NULL;
    if (use_async && log_loop) {
        async_event_loop_stop(log_loop);
        log_loop = NULL;
        use_async = 0;
    }
}

/**
 * @brief Установить минимальный уровень логирования.
 * @param level Минимальный уровень
 */
void ant_log_set_level(ant_log_level_t level) { min_level = level; }

/**
 * @brief Установить пользовательский callback для логирования.
 * @param cb Callback-функция
 */
void ant_log_set_callback(ant_log_callback_t cb) { user_callback = cb; }

// --- Асинхронная обёртка ---
/**
 * @brief Структура для передачи лог-сообщения в event loop
 */
typedef struct {
    ant_log_level_t level;
    char module[ANT_LOG_MAX_MODULE];
    char message[ANT_LOG_MAX_MSG];
} AntLogAsyncMsg;

/**
 * @brief Вывод сообщения (thread-safe)
 */
static void ant_log_output(ant_log_level_t level, const char* module, const char* msg) {
    pthread_mutex_lock(&log_mutex);
    if (user_callback) {
        user_callback(level, module, msg);
    } else {
        if (log_file) {
            fputs(msg, log_file);
            fflush(log_file);
        } else {
            fputs(msg, stderr);
        }
    }
    pthread_mutex_unlock(&log_mutex);
}

/**
 * @brief Форматирование сообщения с цветом, временем и контекстом.
 */
static void ant_log_format(char* out, size_t out_sz, ant_log_level_t level, const char* module, const char* file, int line, const char* func, const char* msg) {
    char timebuf[32];
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", tm_info);
    if (use_colors) {
        snprintf(out, out_sz, "%s[%s] [%s] [%s] %s:%d (%s): %s\033[0m\n",
            level_colors[level], timebuf, level_names[level], module, file, line, func, msg);
    } else {
        snprintf(out, out_sz, "[%s] [%s] [%s] %s:%d (%s): %s\n",
            timebuf, level_names[level], module, file, line, func, msg);
    }
}

// --- Функция для event loop ---
#include "../../../core/types.h"
#include "../../../core/math/mathematica.h"
/**
 * @brief Функция-обёртка для асинхронного вывода логов через event loop.
 */
static Result ant_log_async_func(FunctionParams* params) {
    if (!params || !params->argv || params->argc < 1) {
        fprintf(stderr, "[ant_logger] ERROR: Invalid async log parameters!\n");
        return (Result){ .is_ok = 0 };
    }
    AntLogAsyncMsg* logmsg = (AntLogAsyncMsg*)params->argv[0].value;
    if (!logmsg) {
        fprintf(stderr, "[ant_logger] ERROR: NULL log message in async!\n");
        return (Result){ .is_ok = 0 };
    }
    ant_log_output(logmsg->level, logmsg->module, logmsg->message);
    free(logmsg);
    free(params->argv);
    free(params);
    return (Result){ .is_ok = 1 };
}

/**
 * @brief Основная функция логирования (используйте макросы).
 * @param level Уровень
 * @param module Модуль
 * @param file Файл
 * @param line Строка
 * @param func Функция
 * @param fmt Формат
 * @param ... Аргументы
 */
void ant_log_write(ant_log_level_t level, const char* module, const char* file, int line, const char* func, const char* fmt, ...) {
    if (level < min_level) return;
    if (!module || !file || !func || !fmt) {
        fprintf(stderr, "[ant_logger] ERROR: Invalid log parameters!\n");
        return;
    }
    char user_msg[ANT_LOG_MAX_MSG];
    va_list args;
    va_start(args, fmt);
    vsnprintf(user_msg, sizeof(user_msg), fmt, args);
    va_end(args);

    char final_msg[ANT_LOG_MAX_MSG];
    ant_log_format(final_msg, sizeof(final_msg), level, module, file, line, func, user_msg);

    if (use_async && log_loop) {
        AntLogAsyncMsg* logmsg = (AntLogAsyncMsg*)malloc(sizeof(AntLogAsyncMsg));
        if (!logmsg) {
            fprintf(stderr, "[ant_logger] ERROR: Out of memory for async log!\n");
            return;
        }
        logmsg->level = level;
        strncpy(logmsg->module, module, ANT_LOG_MAX_MODULE-1);
        logmsg->module[ANT_LOG_MAX_MODULE-1] = 0;
        strncpy(logmsg->message, final_msg, ANT_LOG_MAX_MSG-1);
        logmsg->message[ANT_LOG_MAX_MSG-1] = 0;
        // Подготовка параметров для event loop
        FunctionParams* params = (FunctionParams*)malloc(sizeof(FunctionParams));
        if (!params) {
            fprintf(stderr, "[ant_logger] ERROR: Out of memory for async params!\n");
            free(logmsg);
            return;
        }
        params->argc = 1;
        params->argv = (Generic*)malloc(sizeof(Generic));
        if (!params->argv) {
            fprintf(stderr, "[ant_logger] ERROR: Out of memory for async argv!\n");
            free(logmsg);
            free(params);
            return;
        }
        params->argv[0].value = logmsg;
        params->argv[0].type = GENERIC_TYPE_PTR;
        FunctionPipeline* fp = (FunctionPipeline*)malloc(sizeof(FunctionPipeline));
        if (!fp) {
            fprintf(stderr, "[ant_logger] ERROR: Out of memory for async pipeline!\n");
            free(logmsg);
            free(params->argv);
            free(params);
            return;
        }
        fp->func = ant_log_async_func;
        fp->next = NULL;
        async_run(log_loop, fp, params);
    } else {
        ant_log_output(level, module, final_msg);
    }
}
/** @} */ 