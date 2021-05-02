#include <sys/socket.h>
#include <sys/epoll.h>
#include "rpc/accept_event.hpp"
#include "rpc/socket_event.hpp"
#include "event/event_loop.hpp"
#include "utils/debug.hpp"

Acceptor::Acceptor(EventLoop& loop, int listen_sock): 
    Event(loop, listen_sock) { }

Acceptor::~Acceptor() { } // handled by Event::~Event()

void Acceptor::handle_event(uint32_t events) {
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
            Socket *socket_event = new Socket(loop_, client_sock);
            loop_.addEvent((Event *) socket_event, EPOLLIN | EPOLLONESHOT);
        }
    }
}

void Acceptor::handle_deregister() { }