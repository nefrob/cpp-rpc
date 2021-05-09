#include <cstring>
#include <netdb.h>
#include <cassert>
#include "client/client_socket.hpp"
#include "utils/network_utils.hpp"
#include "utils/utils.hpp"
#include "utils/debug.hpp"

int create_client_socket(const char *hostname, 
        const uint16_t port, bool blocking) {
    assert(hostname != NULL);

    int client_fd;
    struct addrinfo hints;
    struct addrinfo *server_info, *p;
    int ret;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP

    ret = getaddrinfo(hostname, std::to_string(port).c_str(), 
        &hints, &server_info);
    if (ret != 0) {
        LOG_ERROR("getaddrinfo failed: %s", gai_strerror(ret));
        return CLIENT_SOCK_ERROR;
    }

    // Loop through addrinfo results and bind to first possible
    int block_type = blocking ? 0 : SOCK_NONBLOCK;
    for (p = server_info; p != NULL; p = p->ai_next) {
        if ((client_fd = socket(p->ai_family, p->ai_socktype | block_type, 
            p->ai_protocol)) < 0) {
            LOG_ERROR("socket open failed: %s", strerror(errno));
            continue;
        }

        if (connect(client_fd, p->ai_addr, p->ai_addrlen) < 0) {
            close_log_err(client_fd);
            LOG_ERROR("connect failed: %s", strerror(errno));
            continue;
        }

        break; // success
    }

    if (p == NULL) {
        LOG_ERROR("client unable to connect to any host address");   
        freeaddrinfo(server_info);
        return CLIENT_SOCK_ERROR;
    }

    LOG_DEBUG("client connecting to: %s", 
        get_ip_addr(p->ai_family, p->ai_addr).c_str());

    freeaddrinfo(server_info);

    return client_fd;
}