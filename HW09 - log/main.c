#include <stdio.h>

#include "log.h"

void dummy_function(void)
{
    LOG_ERROR("Sometimes hit happens. Here is your call stack\n");
}

int main(void)
{
    LOG_DEBUG("2 + 2 = %d\n", 2 + 2);
    log_init("log", INFO);
    LOG_INFO("\tCurrent log level: %d\n\n", current_log_level());
    dummy_function();
    return 0;
}