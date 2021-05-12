/**
 * File: socket_event.hpp
 * ----------------------
 * Defines a socket event to be used with epoll-based event loops.
 */

#pragma once
#include <stddef.h>
#include <memory>
#include <queue>
#include "rpc/message.hpp"
#include "event/event.hpp"

/* Forward declarations (stop cyclic dependencies). */
class EventLoop;
class SocketMessageHandler;

/**
 * Network socket event loop event.
 */
class Socket: public Event, public std::enable_shared_from_this<Socket> {
    public:
        /**
         * Constructs socket handler for use with epoll event loop.
         * 
         * @param loop: loop socket is associated with.
         * @param sockfd: socket file descriptor to handle.
         * @param msg_handler: incoming message handler.
         */
        Socket(EventLoop& loop, int sockfd, SocketMessageHandler& msg_handler);

        /**
         * Deconstructor.
         */
        ~Socket();

        /**
         * Queues a new message to be sent on the socket. Should only
         * be called from the event loop.
         * 
         * @param data: buffer of data to send.
         * @param len: length of data in bytes to copy from `data`.
         */
        void queueMessage(const void *data, size_t len);

        /**
         * Handles socket read/write events.
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

        /* Incoming message handler. */
        SocketMessageHandler& msg_handler_;

        /* Set non-construction and assignment copyable. */
        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;
};