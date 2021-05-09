/**
 * File: server_socket.hpp
 * -----------------------
 * Sets up server socket so it can receive client requests.
 */

#pragma once
#include <cstdint>

 /** 
  * Socket setup failure. 
  */
static const int SERVER_SOCK_ERROR = -1;

/**
 * Server connection backlog. 
 */
static const int DEFAULT_BACKLOG = 16; 

/**
 * Creates and binds server socket to provided IP address (or any if none
 * provided) on the specified port.
 * 
 * @param port: port to accept connections on.
 * @param bind_hostname: target hostname to bind to. If none specified,
 *     the first possible address will be used.
 * @param backlog: backlog connection queue to keep.
 * @param blocking: whether socket should be set up as blocking 
 *     or non-blocking.
 */
int create_server_socket(const uint16_t port, const char *bind_hostname = NULL,
    int backlog = DEFAULT_BACKLOG, bool blocking = true);