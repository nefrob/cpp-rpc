#include <cassert>
#include <netdb.h>
#include <stdio.h>
#include "rpc/server_socket.hpp"
#include "utils/utils.hpp"
#include "utils/debug.hpp"

int create_server_socket(const uint16_t port, 
        const char *bind_hostname, int backlog, bool blocking) {
    int server_fd;
    struct addrinfo hints;
    struct addrinfo *server_info, *p;
    int yes = 1;
    int ret;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE; // fill my IP

    ret = getaddrinfo(bind_hostname, std::to_string(port).c_str(),
        &hints, &server_info);
    if (ret != 0) {
        LOG_ERROR("getaddrinfo failed: %s", gai_strerror(ret));
        return SERVER_SOCK_ERROR;
    }

    // Loop through addrinfo results and bind to first possible
    int block_type = blocking ? 0 : SOCK_NONBLOCK;
    for (p = server_info; p != NULL; p = p->ai_next) {
        if ((server_fd = socket(p->ai_family, p->ai_socktype | block_type, 
            p->ai_protocol)) < 0) {
            LOG_ERROR("socket open failed: %s", strerror(errno));
            continue;
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
            &yes, sizeof(yes)) == -1) {
            LOG_ERROR("setsockopt failed: %s", strerror(errno));
            return SERVER_SOCK_ERROR;
        }

        if (bind(server_fd, p->ai_addr, p->ai_addrlen) < 0) {
            close_log_err(server_fd);
            LOG_ERROR("bind failed: %s.", strerror(errno));
            continue;
        }

        break; // success
    }

    freeaddrinfo(server_info);

    if (p == NULL) {
        LOG_ERROR("server unable to bind to any address");  
        return SERVER_SOCK_ERROR;
    }

    if (listen(server_fd, backlog) < 0) {
        LOG_ERROR("listen failed: %s.", strerror(errno));
        return SERVER_SOCK_ERROR;
    }

    return server_fd;
}