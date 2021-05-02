#include <sys/epoll.h>
#include "rpc/rpc_server.hpp"
#include "rpc/server_socket.hpp"
#include "rpc/accept_event.hpp"
#include "utils/utils.hpp"
#include "utils/network_utils.hpp"

RpcServer::RpcServer(uint16_t port, std::string ip):
    port_(port), ip_(ip), loop_() {

    int listen_sock = create_server_socket(port, ip_.c_str(), DEFAULT_BACKLOG, false);
    if (listen_sock == SERVER_SOCK_ERROR) 
        PANIC("listen socket create failed");

    Acceptor *server_sock = new Acceptor(loop_, listen_sock);
    loop_.addEvent(server_sock, EPOLLIN);

    LOG_DEBUG("RPC server accepting on port: %hu", port);
}

RpcServer::~RpcServer() {
    stop_server();
}

void RpcServer::stop_server() {
    LOG_DEBUG("RPC server %s:%hu shutting down ...",
        get_host_name().c_str(), port_);

    // TODO: close acceptor socket?

    // close all conns?

    loop_.stop();
}