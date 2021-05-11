#include <sys/epoll.h>
#include "rpc/rpc_server.hpp"
#include "rpc/server_socket.hpp"
#include "rpc/accept_event.hpp"
#include "utils/utils.hpp"
#include "utils/network_utils.hpp"
#include "rpc/message.hpp"

/* Static forward declarations. */
static struct message *echo_response(struct message *request) {
    return request;
}

RpcServer::RpcServer(uint16_t port, std::string ip):
    port_(port), ip_(ip), loop_(), running_(true),
    rpc_responder_(loop_, echo_response) {

    int listen_sock = create_server_socket(port, ip_.c_str(), DEFAULT_BACKLOG, false);
    if (listen_sock == SERVER_SOCK_ERROR) 
        PANIC("listen socket create failed");

    std::shared_ptr<Acceptor> server_sock = 
        std::make_shared<Acceptor>(loop_, listen_sock, rpc_responder_);
    loop_.addEvent(std::move(server_sock), EPOLLIN);

    LOG_DEBUG("RPC server accepting on port: %hu", port);
}

RpcServer::~RpcServer() {
    stop();
}

void RpcServer::stop() {
    if (!running_) return;

    LOG_DEBUG("RPC server %s:%hu shutting down ...",
        get_host_name().c_str(), port_);

    // FIXME: event shared pointers go out of scope when loop is stopped
    // so will be automatically closed down?
    loop_.stop();

    running_ = false;
}