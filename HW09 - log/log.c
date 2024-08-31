#include <execinfo.h>
#include <stdlib.h>
#include <stdio.h>

#include "log.h"

static LogLevel LOG_LEVEL = DEBUG;
static FILE *LOG_FILE = NULL;

LogLevel current_log_level() { return LOG_LEVEL; }

void log_trace(void)
{
    void *array[TRACE_DEPTH];
    int size = backtrace(array, TRACE_DEPTH);
    char **strings = backtrace_symbols(array, size);
    FILE *log_file = LOG_FILE;
    if (log_file == NULL)
        log_file = stderr;
    if (strings != NULL)
    {
        fprintf(log_file, "Traceback of %d stack frames.\n", size);
        for (int i = 0; i < size; i++)
            fprintf(log_file, "\t%s\n", strings[i]);
    }

    free(strings);
}
