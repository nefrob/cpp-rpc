#include <cstdarg>
#include <thread>
#include <sstream>
#include "utils/debug.hpp"

/* Printf coloring constants.
 * Ref: https://gist.github.com/abritinthebay/d80eb99b2726c83feb0d97eab95206c4.*/
#define BLUE  "\x1B[34m"
#define RED "\x1b[31m"
#define YELLOW "\x1b[33m"
#define RESET "\033[0m"

/* Current log level. */
static LogLevel global_log_level = DEBUG;

/* Checks if specified log level is within logging policy log level. */
static bool should_log(LogLevel level) {
    return level <= global_log_level && level != SILENT;
}

std::string format_str(const char *fmt, ...) {
    assert(fmt != NULL);

    std::string s;
    va_list args;
    va_start(args, fmt);

    size_t buf_size = 256;
    while (true) {
        char buf[buf_size];
        
        va_list args_copy; /* vsnprintf trashes va_list, so copy. */
        va_copy(args_copy, args);
        int n = vsnprintf(buf, buf_size, fmt, args_copy);
        assert(n >= 0);
        va_end(args_copy);

        if (n < buf_size) {
            s = buf;
            break;
        }

        buf_size = n + 1;
    }

    va_end(args);
    return s;
}

void set_log_level(LogLevel level) {
    assert(level >= SILENT && level <= DEBUG);

    std::lock_guard<std::mutex> log_lg(log_lock);
    global_log_level = level;
}

void log(LogLevel level, const char *file, int line, 
    const char *func, const char *msg) {
    assert(level >= SILENT && level <= DEBUG);
    assert(file != NULL);
    assert(func != NULL);
    assert(msg != NULL);

    std::lock_guard<std::mutex> log_lg(log_lock);

    if (!should_log(level)) return;

    std::ostringstream ss;
    ss << std::this_thread::get_id();
    const char *tid = ss.str().c_str();

    if (level == ERROR) {
        fprintf(stderr, RED "[ERROR][%s:%d][%s][%s]: %s\n" RESET, 
            file, line, func, tid, msg);
    } else if (level == WARN) {
        fprintf(stderr, YELLOW "[WARN][%s:%d][%s][%s]: %s\n" RESET, 
            file, line, func, tid, msg);
    } else if (level == DEBUG) {
        fprintf(stderr, BLUE "[DEBUG][%s:%d][%s][%s]: %s\n" RESET, 
            file, line, func, tid, msg);
    } else {
        fprintf(stderr, 
            RED "[ERROR][%s:%d][%s][%s]: INVALID LOGLEVEL %d\n" RESET, 
            file, line, func, tid, level);
    }

    fflush(stderr);
}
