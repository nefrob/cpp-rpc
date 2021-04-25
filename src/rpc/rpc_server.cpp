#include "rpc/rpc_server.hpp"
#include "rpc/server_socket.hpp"
#include "utils/utils.hpp"
#include "utils/network_utils.hpp"

RpcServer::RpcServer(uint16_t port, std::string ip):
    port_(port), ip_(ip), loop_() {

    int listen_sock = create_server_socket(port, ip_.c_str(), DEFAULT_BACKLOG, false);
    if (listen_sock == SERVER_SOCK_ERROR) 
        PANIC("listen socket create failed");

    // TODO: create accept socket
    // Event *server_sock = new Event(loop, listen_sock);
    // loop.addEvent(server_sock, ...);

    LOG_DEBUG("RPC server accepting on port: %hu", port);

    // TODO: set running
}

RpcServer::~RpcServer() {
    stop_server();
}

void RpcServer::stop_server() {
    LOG_DEBUG("RPC server %s:%hu shutting down ...",
        get_host_name().c_str(), port_);

    loop_.stop();

    // TODO:
}