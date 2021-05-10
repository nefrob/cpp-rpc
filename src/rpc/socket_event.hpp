/**
 * File: socket_event.hpp
 * ----------------------
 * TODO:
 */

#pragma once
#include <stddef.h>
#include <memory>
#include <queue>
#include "rpc/message.hpp"
#include "event/event.hpp"

/* Forward declarations (stop cyclic dependencies). */
class EventLoop;
class RpcResponder;

/**
 * Network socket event loop event.
 */
class Socket: public Event, public std::enable_shared_from_this<Socket> {
    public:
        /**
         * EPOLLIN | EPOLLONESHOT
         * 
         * @param loop:
         * @param sockfd: 
         */
        Socket(EventLoop& loop, int sockfd, RpcResponder& responder);

        /**
         * Deconstructor.
         */
        ~Socket();

        /**
         * Queues a new message to be sent on the socket.
         * 
         * @param data: buffer of data to send.
         * @param len: length of data in bytes to copy from `data`.
         */
        void queueMessage(const void *data, size_t len);

        /**
         * 
         *
         * @param events: epoll set event flags.
         */
        void handleEvent(uint32_t events);

    private:
        /* Called by handleEvent when epoll returns a read event on sockfd. 
           Executes within event loop. */
        bool handleReadable();

        /* Called by handleEvent when epoll returns a write event on sockfd. 
           Executes within event loop. */
        bool handleWriteable();

        /* Queue of malloc'd messages waiting to be sent on the socket. */
        std::queue<struct message *> pending_msgs_;

        /* Message currently being sent. */
        struct message *send_msg_;

        /* Offset of already sent bytes into active send message. */
        size_t send_offset_;

        /* Message currently being received. */
        struct message *recv_msg_;

        /* Offset of already received bytes into active receive message. */
        size_t recv_offset_;

        /* */
        RpcResponder& responder_;

        /* Set non-construction and assignment copyable. */
        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;
};