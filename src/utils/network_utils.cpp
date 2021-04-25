#include <cassert>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <limits.h>
#include "utils/network_utils.hpp"

void *get_sockaddr_in(struct sockaddr *sa) {
    assert(sa != NULL);

    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in *) sa)->sin_addr);

    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

std::string get_ip_addr(int family, struct sockaddr *sa) {
    assert(sa != NULL);

    char ip_addr[INET6_ADDRSTRLEN];
    const char *ret = 
        inet_ntop(family, get_sockaddr_in(sa), ip_addr, sizeof(ip_addr));
    if (ret == NULL) return "";
    return std::string(ip_addr);
}

std::string get_host_name() {
    char host_name[HOST_NAME_MAX];
    host_name[HOST_NAME_MAX - 1] = '\0';
    gethostname(host_name, HOST_NAME_MAX - 1);
    return std::string(host_name);
}