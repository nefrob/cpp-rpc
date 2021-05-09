/**
 * File: client_socket.hpp
 * -----------------------
 * Interface clients can use to connect to remote host on port.
 */

#pragma once

/** 
 * Remote connection failure. 
 */
const static int CLIENT_SOCK_ERROR = -1; 

/**
 * Establishes connection with host on specified port, returning
 * socket descriptor for communication.
 * 
 * @param hostname: host to connect to.
 * @param port: port to connect on.
 * @param blocking: whether socket should be set up as blocking 
 *     or non-blocking.
 */
int create_client_socket(const char *hostname, const uint16_t port,
    bool blocking = true);