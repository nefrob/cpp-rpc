#include <stdlib.h> 
#include <sys/epoll.h>
#include <sys/socket.h>
#include "rpc/socket_event.hpp"
#include "rpc/socket_msg_handler.hpp"
#include "event/event_loop.hpp"
#include "utils/network_utils.hpp"
#include "utils/debug.hpp"

Socket::Socket(EventLoop& loop, int sockfd, 
    SocketMessageHandler& msg_handler): 
    Event(loop, sockfd),
    send_msg_(NULL), send_offset_(0), recv_offset_(0), recv_msg_(NULL),
    msg_handler_(msg_handler) {
}

Socket::~Socket() {
    struct message *msg;

    if (send_msg_ != NULL) {
        if (send_msg_->data != NULL) free(send_msg_->data);
        free(send_msg_);
    }

    if (recv_msg_ != NULL) {
        if (recv_msg_->data != NULL) free(recv_msg_->data);
        free(recv_msg_);
    }

    while (pending_msgs_.size() > 0) {
        msg = pending_msgs_.front();
        free(msg->data);
        free(msg);
        
        pending_msgs_.pop();
    }
}

void Socket::queueMessage(const void *data, size_t len) {
    assert(loop_.inLoopThread());
    assert(data != NULL);
    assert(len != 0);

    struct message *msg = (struct message *) malloc(sizeof(struct message));
    if (msg == NULL) PANIC("malloc fail");

    msg->len = len;
    msg->data = malloc(msg->len);
    if (msg->data == NULL) PANIC("malloc fail");

    memcpy(msg->data, data, msg->len);

    pending_msgs_.push(msg);
    loop_.updateEvent(this, EPOLLOUT | EPOLLIN | EPOLLONESHOT);
}

void Socket::handleEvent(uint32_t events) {
    assert(loop_.inLoopThread());

    if ((events & EPOLLERR) || (events & EPOLLHUP)) {
        loop_.removeEvent(this);
        return;
    }
    
    if ((events & EPOLLIN) && !handleReadable()) {
        loop_.removeEvent(this);
        return;
    }

    if ((events & EPOLLOUT) && !handleWriteable()) {
        loop_.removeEvent(this);
        return;
    }
    
    int flags = EPOLLIN | EPOLLONESHOT;
    if (pending_msgs_.size() > 0) flags |= EPOLLOUT;
    loop_.updateEvent(this, flags);
}

bool Socket::handleReadable() {
    assert(loop_.inLoopThread());

    ssize_t ret;

    while (true) {
        if (recv_msg_ == NULL) {
            recv_msg_ = (struct message *) malloc(sizeof(struct message));
            if (recv_msg_ == NULL) PANIC("malloc fail");

            recv_msg_->data = NULL;
        }

        if (recv_offset_ < sizeof(size_t)) {
            ret = recv_all(fd(), (uint8_t *) &(recv_msg_->len) + recv_offset_,
                sizeof(size_t) - recv_offset_, MSG_DONTWAIT);
            
            if (ret < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) return true;
                return false;
            } else if (ret == 0) return false;

            recv_offset_ += ret;
            if (recv_offset_ < sizeof(recv_msg_->len)) break;

            recv_msg_->data = malloc(recv_msg_->len);
            if (recv_msg_->data == NULL) PANIC("malloc fail");
        }

        size_t msg_offset = recv_offset_ - sizeof(size_t);
        ret =  recv_all(fd(), (uint8_t *) recv_msg_->data + msg_offset,
            recv_msg_->len - msg_offset, MSG_DONTWAIT);
        
        if (ret < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) return true;
            return false;
        } else if (ret == 0) return false;

        recv_offset_ += ret;
        if (msg_offset + ret == recv_msg_->len) {
            LOG_DEBUG("Message received from %s", get_peer_ip(fd()).c_str());

            msg_handler_.handleMessage(weak_from_this(), recv_msg_);

            recv_offset_ = 0;
            recv_msg_ = NULL;
        }
    }

    return true;
}

bool Socket::handleWriteable() {
    assert(loop_.inLoopThread());
    
    ssize_t ret;

    while (true) {
        if (send_msg_ == NULL && !pending_msgs_.empty()) {
            send_msg_ = pending_msgs_.front();
            pending_msgs_.pop();
        } else break;

        if (send_offset_ < sizeof(send_msg_->len)) {
            ret = send_all(fd(), (uint8_t *) &(send_msg_->len) + send_offset_,
                sizeof(send_msg_->len) - send_offset_, MSG_DONTWAIT);
            
            if (ret < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) return true;
                return false;
            } else if (ret == 0) return false;

            send_offset_ += ret;
            
            if (send_offset_ < sizeof(send_msg_->len)) break;
        }

        size_t msg_offset = send_offset_ - sizeof(send_msg_->len);
        ret = send_all(fd(), (uint8_t *) send_msg_->data + msg_offset,
            send_msg_->len - msg_offset, MSG_DONTWAIT);
        
        if (ret < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) return true;
            return false;
        } else if (ret == 0) return false;

        send_offset_ += ret;
        
        if (msg_offset + ret == send_msg_->len) {
            LOG_DEBUG("Message sent to peer %s", get_peer_ip(fd()).c_str());

            free(send_msg_->data);
            free(send_msg_);
            send_msg_ = NULL;
            send_offset_ = 0;
        } else break;
    }

    return true;
}