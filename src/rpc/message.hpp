/**
 * File: message.hpp
 * -----------------
 * Defines message for send/receive functionality over network 
 * socket file descriptor.
 */

#pragma once
#include <sys/types.h>

/**
 * Container for simple message.
 */
struct message {
    /**
     * Message data.
     */
    void *data;
    
    /**
     * Message length in bytes.
     */
    size_t len;
};