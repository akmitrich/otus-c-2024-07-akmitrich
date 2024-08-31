#include <stdio.h>

#include "log.h"

void dummy_function(void)
{
    log_trace();
}

int main(void)
{
    printf("\tCurrent log level: %d\n\n", current_log_level());
    dummy_function();
    return 0;
}