# AntLogger: Документация

## Описание

**AntLogger** — это мощная, потокобезопасная, кроссплатформенная система логирования для C и C++ с поддержкой:
- Уровней логирования (DEBUG, INFO, WARNING, ERROR, FATAL)
- Модулей (строковый идентификатор источника)
- Цветного вывода (ANSI-цвета)
- Асинхронного режима (через event loop из core/async)
- Callback для интеграции с внешними системами
- Вывода в файл или stderr
- Потокобезопасности

---

## Быстрый старт

```c
#include "ant_logger.h"

int main() {
    ant_log_init(NULL); // лог в stderr
    ant_log_set_level(ANT_LOG_DEBUG);
    ant_log_enable_colors(true);
    ant_log_enable_async(true); // асинхронный режим

    ANT_LOG_INFO("Hello, world!");
    ANT_LOG_WARNING_M("network", "Connection lost: %s", "timeout");
    ANT_LOG_ERROR("Critical error: %d", 42);

    ant_log_shutdown();
    return 0;
}
```

---

## Основные возможности

- **Уровни:** ANT_LOG_DEBUG, ANT_LOG_INFO, ANT_LOG_WARNING, ANT_LOG_ERROR, ANT_LOG_FATAL
- **Модули:** логируйте с помощью макросов `ANT_LOG_*_M(module, ...)`
- **Цвет:** включается через `ant_log_enable_colors(true)`
- **Асинхронность:** включается через `ant_log_enable_async(true)`
- **Callback:** `ant_log_set_callback(my_callback)`
- **Файл:** `ant_log_init("log.txt")`

---

## Примеры использования

### Логирование с модулем
```c
ANT_LOG_INFO_M("db", "Database connected: %s", dbname);
```

### Асинхронный режим
```c
ant_log_enable_async(true);
ANT_LOG_ERROR("Async error: %s", msg);
ant_log_enable_async(false); // дождаться завершения очереди
```

### Callback
```c
void my_cb(ant_log_level_t lvl, const char* mod, const char* msg) {
    // отправить в syslog, Qt, etc.
}
ant_log_set_callback(my_cb);
```

---

## Иллюстрированная схема работы

```
[Потоки приложения]
      |         |         |
      v         v         v
   ant_log_write(...)
      |         |         |
      +----> [Асинхронная очередь] -----> [Event loop] -----> [ant_log_output]
      |         |         |
      +----> [Синхронно]  -----> [ant_log_output]
```

---

## FAQ

**Q: Что будет, если лог-файл не открылся?**
- Сообщения будут писаться в stderr.

**Q: Как отключить цвет?**
- `ant_log_enable_colors(false);`

**Q: Как логировать только WARNING и выше?**
- `ant_log_set_level(ANT_LOG_WARNING);`

**Q: Как работает асинхронность?**
- Сообщения помещаются в очередь и обрабатываются worker-потоком через event loop из core/async.

**Q: Потокобезопасно ли?**
- Да, используется mutex.

**Q: Как логировать из C++?**
- API полностью совместим с C++.

---

## Типичные ошибки

- **Передача NULL в параметры** — будет выведено сообщение об ошибке в stderr.
- **Недостаточно памяти** — сообщение не будет залогировано, будет выведено в stderr.
- **Асинхронный режим выключен, но очередь не обработана** — сообщения пишутся синхронно.

---

## Контакты и поддержка

- Вопросы и баги: создавайте issue в репозитории проекта.
- Автор: twelvefaced 