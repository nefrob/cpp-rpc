/**
 * File: network_utils.hpp
 * -----------------------
 * Network connection utilities.
 */

#pragma once
#include <string>

/**
 * Invalid/uninitialized socket value. 
 */
static const int INVALID_SOCKET = -1;

/**
 * Return socket address struct for either IPv4 or IPv6 type.
 * 
 * @param sa: sockaddr with IP address family type.
 */
void *get_sockaddr_in(struct sockaddr *sa);

/**
 * Returns string IP address from binary form.
 * 
 * @param family: IP address type (ex. IPv4 or IPv6).
 * @param sa: sockaddr storing ip address.
 */
std::string get_ip_addr(int family, struct sockaddr *sa);

/**
 * Return local hostname.
 */
std::string get_host_name();

/**
 * Return peer ip address for given socket file descriptor.
 * 
 * @param sockfd: socket connection file descriptor, ex. 
 *                from call to "accept".
 */
std::string get_peer_ip(int sockfd);

/**
 * Returns whether peer has closed the connection on sockfd
 * without blocking (i.e. checking if recv returns zero).
 * 
 * @param sockfd: socket to check.
 */
bool connection_alive(int sockfd);

/**
 * Send len bytes from buffer to socket file descriptor.
 * Returns bytes sent or zero if the connection was closed. 
 * Returns -1 on error.
 * 
 * Note: may block until sufficient space in kernel send buffer.
 * Can be avoided by using non-blocking MSG_DONTWAIT flag.
 * 
 * @param sockfd: socket to send to.
 * @param buf: buffer to send message from.
 * @param len: number of bytes to send.
 * @param flags: send flags.
 */
ssize_t send_all(int sockfd, const void *buf, size_t len, int flags = 0);

/**
 * Receive len bytes from socket file destrictor, 
 * storing them in buffer. Returns bytes received or 
 * zero if the connection was closed. Returns -1 
 * on error. 
 * 
 * Note: may block waiting for data to arrive. Can be avoided
 * by using non-blocking MSG_DONTWAIT flag.
 * 
 * @param sockfd: socket to receive on.
 * @param buf: buffer to store received message in.
 * @param len: number of bytes to store in buf.
 * @param flags: recv flags.
 */
ssize_t recv_all(int sockfd, void *buf, size_t len, int flags = 0);