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

std::string get_peer_ip(int sockfd) {
    assert(sockfd != INVALID_SOCKET);

    struct sockaddr_storage client_addr;
    socklen_t addr_len = sizeof(client_addr);

    if (getpeername(sockfd, (struct sockaddr *) &client_addr, 
        &addr_len) < 0) {
        return "";
    }

    return get_ip_addr(client_addr.ss_family, 
        (struct sockaddr *) &client_addr);
}

bool connection_alive(int sockfd) {
    assert(sockfd != INVALID_SOCKET);

    char buf[1];
    return recv(sockfd, buf, 1, MSG_DONTWAIT | MSG_PEEK) != 0;
}

ssize_t send_all(int sockfd, const void *buf, size_t len, int flags) {
    assert(sockfd != INVALID_SOCKET);
    assert(buf != NULL);

    ssize_t total_sent = 0;
    ssize_t n_sent;
    void *offset_buf = (void *) buf;
    
    while (total_sent < len) {
        n_sent = send(sockfd, offset_buf, len - total_sent, flags);

        if (n_sent < 0 && total_sent > 0 && (flags & MSG_DONTWAIT)
            && (errno == EAGAIN || errno == EWOULDBLOCK)) return total_sent;
        else if (n_sent <= 0) return n_sent;

        total_sent += n_sent;
        offset_buf = (uint8_t *) buf + total_sent;
    }

    return total_sent;
}

ssize_t recv_all(int sockfd, void *buf, size_t len, int flags) {
    assert(sockfd != INVALID_SOCKET);
    assert(buf != NULL);

    ssize_t total_read = 0;
    ssize_t n_read;
    void *offset_buf = buf;

    while (total_read < len) {
        n_read = recv(sockfd, offset_buf, len - total_read, flags);

        if (n_read < 0 && total_read > 0 && (flags & MSG_DONTWAIT)
            && (errno == EAGAIN || errno == EWOULDBLOCK)) return total_read;
        else if (n_read <= 0) return n_read;

        total_read += n_read;
        offset_buf = (uint8_t *) buf + total_read;
    }

    return total_read;
}