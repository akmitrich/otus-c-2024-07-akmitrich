#ifndef __LOG_H
#define __LOG_H

#define TRACE_DEPTH 1024

typedef enum
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
} LogLevel;

LogLevel current_log_level();
void log_trace(void);
#endif