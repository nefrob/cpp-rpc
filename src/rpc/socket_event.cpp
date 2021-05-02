#include <sys/epoll.h>
#include <sys/socket.h>
#include "rpc/socket_event.hpp"
#include "event/event_loop.hpp"
#include "utils/network_utils.hpp"
#include "utils/debug.hpp"

Socket::Socket(EventLoop& loop, int sockfd): 
    Event(loop, sockfd),
    send_msg(NULL), send_offset(0), recv_offset(0){
    
    recv_msg.data = NULL;
    recv_msg.len = 0;
}

Socket::~Socket() {
    struct message *msg;
    
    while (pending_msgs.size() > 0) {
        msg = pending_msgs.front();
        free(msg->data);
        free(msg);
        
        pending_msgs.pop();
    }
}

void Socket::queue_message(const void *data, size_t len) {
    assert(data != NULL);
    assert(len != 0);

    struct message *msg = (struct message *) malloc(sizeof(struct message));
    if (msg == NULL) PANIC("malloc fail");

    msg->data = malloc(msg->len);
    if (msg->data == NULL) PANIC("malloc fail");

    msg->len = len;
    memcpy(msg->data, data, msg->len);

    // TODO: bind to run in event loop, add to internal
    
    pending_msgs.push(msg);
    loop_.updateEvent(this, EPOLLOUT | EPOLLIN | EPOLLONESHOT);
}

void Socket::handle_event(uint32_t events) {
    if ((events & EPOLLERR) || (events & EPOLLHUP)) {
        loop_.removeEvent(this);
        return;
    }
    
    if ((events & EPOLLIN) && !handle_readable()) {
        loop_.removeEvent(this);
        return;
    }

    if ((events & EPOLLOUT) && !handle_writeable()) {
        loop_.removeEvent(this);
        return;
    }
    
    int flags = EPOLLIN | EPOLLONESHOT;
    if (pending_msgs.size() > 0) flags |= EPOLLOUT;
    loop_.updateEvent(this, flags);
}

void Socket::handle_deregister() {
    // TODO: nothing?
}

bool Socket::handle_readable() {
    ssize_t ret;

    while (true) {
        if (recv_offset < sizeof(size_t)) {
            ret = recv_all(fd(), (uint8_t *) &(recv_msg.len) + recv_offset,
                sizeof(size_t) - recv_offset, MSG_DONTWAIT);
            
            if (ret < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) return true;
                return false;
            } else if (ret == 0) return false;

            recv_offset += ret;
            if (recv_offset < sizeof(size_t)) break;

            recv_msg.data = malloc(recv_msg.len);
            if (recv_msg.data == NULL) PANIC("malloc fail");
        }

        size_t msg_offset = recv_offset - sizeof(size_t);
        ret =  recv_all(fd(), (uint8_t *) recv_msg.data + msg_offset,
            recv_msg.len - msg_offset, MSG_DONTWAIT);
        
        if (ret < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) return true;
            return false;
        } else if (ret == 0) return false;

        recv_offset += ret;
        if (msg_offset + ret == recv_msg.len) {
            LOG_DEBUG("message received from %s", get_peer_ip(fd()).c_str());

            // TODO: async queue and get rpc response since may need io
            // likely need to provide with shared pointer reference from loop
            // struct message *response = RpcGetter.(this, recv_msg); // frees message

            recv_offset = 0;
        }
    }

    return true;
}

bool Socket::handle_writeable() {
    ssize_t ret;

    while (true) {
        if (send_msg == NULL && !pending_msgs.empty()) {
            send_msg = pending_msgs.front();
            pending_msgs.pop();
        }

        if (send_offset < sizeof(send_msg->len)) {
            ret = send_all(fd(), (uint8_t *) &(send_msg->len) + send_offset,
                sizeof(send_msg->len) - send_offset, MSG_DONTWAIT);
            
            if (ret < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) return true;
                return false;
            } else if (ret == 0) return false;

            send_offset += ret;
            
            if (send_offset < sizeof(send_msg->len)) break;
        }

        size_t msg_offset = send_offset - sizeof(send_msg->len);
        ret = send_all(fd(), (uint8_t *) send_msg->data + msg_offset,
            send_msg->len - msg_offset, MSG_DONTWAIT);
        
        if (ret < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) return true;
            return false;
        } else if (ret == 0) return false;

        send_offset += ret;
        
        if (msg_offset + ret == send_msg->len) {
            LOG_DEBUG("message sent to peer");

            free(send_msg->data);
            send_msg = NULL;
            send_offset = 0;
        } else break;
    }

    return true;
}