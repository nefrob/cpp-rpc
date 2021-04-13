/**
 * File: debug.hpp
 * ---------------
 * General debugging/logging tools.
 */

#pragma once
#include <string>
#include <cstring> // strchr
#include <mutex>
#include <cassert>

/**
 * Defines levels debug logging can take on.
 */
enum LogLevel {
    SILENT = 0, /* Disable all log messages. */
    ERROR = 1,  /* For unrecoverable issues. */
    WARN = 2,   /* For recoverable issues. */
    DEBUG = 3   /* General system debug logging. */
};

/**
 * Parses filename from file path.
 */
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

/**
 * Returns printf-style string according to variadic arguments
 * and specified format.
 * 
 * @param fmt: printf style format.
 * @param ...: var args.
 */
std::string format_str(const char *fmt, ...) 
    __attribute__((format(printf, 1, 2)));

/**
 * Lock for logging.
 */
static std::mutex log_lock;

/**
 * Sets current logging level.
 * 
 * Note: is thread safe.
 * 
 * @param level: log level to set to.
 */
void set_log_level(LogLevel level);

/**
 * Prints a message with useful debugging information.
 * 
 * Note: is thread safe.
 * 
 * @param level: log level.
 * @param file: file name.
 * @param line: line in file.
 * @param func: function in file.
 * @param msg: debug message to print.
 */
void log(LogLevel level, const char *file, int line, 
    const char *func, const char *msg);

/**
 * Logs message at specified log level according to format.
 */
#define LOG(LEVEL, FMT, ...) \
    log(LEVEL, __FILENAME__, __LINE__, __func__, \
    format_str(FMT, ##__VA_ARGS__).c_str())

/**
 * Logs error message.
 */
#define LOG_ERROR(FMT, ...) LOG(ERROR, FMT, ##__VA_ARGS__)

/**
 *  Logs warning message.
 */
#define LOG_WARN(FMT, ...) LOG(WARN, FMT, ##__VA_ARGS__)

/**
 * Logs debug message.
 */
#define LOG_DEBUG(FMT, ...) LOG(DEBUG, FMT, ##__VA_ARGS__)

/**
 * Logs error message then aborts process.
 */
#define PANIC(FMT, ...) do { \
    LOG_ERROR("PANIC: " FMT, ##__VA_ARGS__); \
    abort(); \
} while (0)

/**
 * Used to mark code that cannot be reached.
 */
#define UNREACHABLE() assert(false)

/**
 * Mark unused function parameter;
 */
#define UNUSED __attribute__ ((unused))
