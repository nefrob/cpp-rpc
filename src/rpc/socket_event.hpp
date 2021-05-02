/**
 * File: socket_event.hpp
 * ----------------------
 * TODO:
 */

#pragma once
#include <stddef.h>
#include <queue>
#include "rpc/message.hpp"
#include "event/event.hpp"

/* Forward declarations (stop cyclic dependencies). */
class EventLoop;

/**
 * Network socket event loop event.
 */
class Socket: private Event {
    public:
        /**
         * EPOLLIN | EPOLLONESHOT)
         */
        Socket(EventLoop& loop, int sockfd);

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
        void queue_message(const void *data, size_t len);

        /**
         * 
         *
         * @param: epoll set event flags.
         */
        void handle_event(uint32_t events);

        /**
         * 
         */
        void handle_deregister();

    private:
        /**
         * Abstract method called by handle_event when epoll returns a 
         * read event on sockfd. Executes within event loop.
         */
        // virtual bool handle_readable() = 0;
        bool handle_readable();

        /**
         * Abstract method called by handle_event when epoll returns a 
         * write event on sockfd. Executes within event loop.
         */
        // virtual bool handle_writeable() = 0;
        bool handle_writeable();

        /* Queue of messages waiting to be sent on the socket. */
        std::queue<struct message *> pending_msgs;

        /* Message currently being sent. */
        struct message *send_msg;

        /* Offset of already sent bytes into active send message. */
        size_t send_offset;

        /* Message currently being received. */
        struct message recv_msg;

        /* Offset of already received bytes into active receive message. */
        size_t recv_offset;

        /* Set non-construction and assignment copyable. */
        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;
};