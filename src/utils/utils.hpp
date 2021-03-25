/**
 * File: utils.hpp
 * ---------------
 * General utility functions.
 */

#pragma once
#include <unistd.h>
#include "utils/debug.hpp"

/**
 * Calls close on provided file descriptor, logging
 * any errors.
 * 
 * @param fd: file descriptor to close.
 */
inline void close_log_err(int fd) {
    if (close(fd) == -1) LOG_ERROR("close failed: %s", strerror(errno));
}