/**
 * File: network_utils.hpp
 * -----------------------
 * Network connection utilities.
 */

#pragma once
#include <string>

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