#include "ant_logger.h"
#include <stdio.h>
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