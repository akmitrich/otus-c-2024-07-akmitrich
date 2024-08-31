#include <execinfo.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "log.h"

#define TRACE_DEPTH 1024

static LogLevel LOG_LEVEL = DEBUG;
static FILE *LOG_FILE = NULL;

LogLevel current_log_level() { return LOG_LEVEL; }
FILE *current_log_file()
{
    if (LOG_FILE)
        return LOG_FILE;
    else
        return stderr;
}

void log_init(const char *log_file_path, LogLevel log_level)
{
    LOG_LEVEL = log_level;
    LOG_FILE = fopen(log_file_path, "w");
    if (LOG_FILE == NULL)
        LOG_ERROR("Failed to open %s for writing. Log to `stderr`.");
}

void log_done(void)
{
    if (LOG_FILE)
        fclose(LOG_FILE);
}

void log_trace(void)
{
    void *return_addresses[TRACE_DEPTH];
    int size = backtrace(return_addresses, TRACE_DEPTH);
    char **trace_entries = backtrace_symbols(return_addresses, size);
    FILE *log_file = current_log_file();
    if (trace_entries)
    {
        fprintf(log_file, "Traceback of %d stack frames.\n", size);
        // First two entries are `log_trace` and `log_msg` so we ignore them both
        for (int i = 2; i < size; i++)
            fprintf(log_file, "\t%s\n", trace_entries[i]);
        free(trace_entries);
    }
}

void log_msg(LogLevel level, const char *filename, int linenumber, const char *fmt, ...)
{
    if (level >= LOG_LEVEL)
    {
        FILE *log_file = current_log_file();
        switch (level)
        {
        case DEBUG:
            fprintf(log_file, "[DEBUG]");
            break;
        case INFO:
            fprintf(log_file, "[INFO]");
            break;
        case WARNING:
            fprintf(log_file, "[WARNING]");
            break;
        case ERROR:
            fprintf(log_file, "[ERROR]");
            break;

        default:
            break;
        }
        fprintf(log_file, " (%s @ %d) ", filename, linenumber);
        va_list args;
        va_start(args, fmt);
        vfprintf(log_file, fmt, args);
        va_end(args);
        if (level == ERROR)
            log_trace();
    }
}
