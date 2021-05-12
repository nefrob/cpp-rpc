#include <sys/socket.h>
#include <sys/epoll.h>
#include "rpc/accept_event.hpp"
#include "rpc/socket_event.hpp"
#include "event/event_loop.hpp"
#include "utils/debug.hpp"
#include "utils/network_utils.hpp"

Acceptor::Acceptor(EventLoop& loop, int listen_sock, 
    SocketMessageHandler& msg_handler): 
    Event(loop, listen_sock), msg_handler_(msg_handler) { }

Acceptor::~Acceptor() { } // handled by Event::~Event()

void Acceptor::handleEvent(uint32_t events) {
    struct sockaddr_storage client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_sock;

    while (true) {
        client_sock = accept4(fd(), (struct sockaddr *) &client_addr, 
            &addr_len, SOCK_NONBLOCK);

        if (client_sock == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            
            LOG_ERROR("accept failed: %s", strerror(errno));
            return;
        } else {
            LOG_DEBUG("New connection from %s", get_peer_ip(client_sock).c_str());

            std::shared_ptr<Socket> socket_event = 
                std::make_shared<Socket>(loop_, client_sock, msg_handler_);
            loop_.addEvent(std::move(socket_event), EPOLLIN | EPOLLONESHOT);
        }
    }
}