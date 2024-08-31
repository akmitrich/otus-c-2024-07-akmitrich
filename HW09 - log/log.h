#ifndef __LOG_H
#define __LOG_H

typedef enum
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
} LogLevel;

#define LOG_ERROR(...)                                   \
    do                                                   \
    {                                                    \
        log_msg(ERROR, __FILE__, __LINE__, __VA_ARGS__); \
    } while (0)

#define LOG_WARNING(...)                                   \
    do                                                     \
    {                                                      \
        log_msg(WARNING, __FILE__, __LINE__, __VA_ARGS__); \
    } while (0)

#define LOG_INFO(...)                                   \
    do                                                  \
    {                                                   \
        log_msg(INFO, __FILE__, __LINE__, __VA_ARGS__); \
    } while (0)

#define LOG_DEBUG(...)                                   \
    do                                                   \
    {                                                    \
        log_msg(DEBUG, __FILE__, __LINE__, __VA_ARGS__); \
    } while (0)

LogLevel current_log_level();
void log_init(const char *log_file_path, LogLevel log_level);
void log_done(void);
void log_msg(LogLevel level, const char *filename, int linenumber, const char *fmt, ...);
#endif