/**
 * File: accept_event.hpp
 * ----------------------
 * Defines an accept socket event to be used with epoll-based
 * event loops.
 */

#pragma once
#include "event/event.hpp"
#include "rpc/socket_msg_handler.hpp"

/* Forward declarations (stop cyclic dependencies). */
class EventLoop;

/**
 * Accept socket event loop event.
 */
class Acceptor: public Event {
    public:

        /**
         * Constructs server accept socket for use with epoll event loop.
         * 
         * @param loop: loop socket is associated with.
         * @param listen_sock: server socket to listen for events on.
         * @param msg_handler: new connection incoming message handler.
         */
        Acceptor(EventLoop& loop, int listen_sock, SocketMessageHandler& msg_handler);

        /**
         * Deconstructor.
         */
        ~Acceptor();

        /**
         * Handles socket read events.
         *
         * @param: epoll set event flags.
         */
        void handleEvent(uint32_t events);

    private:
        /* Socket incoming message handler for new connections. */
        SocketMessageHandler& msg_handler_;

        /* Set non-construction and assignment copyable. */
        Acceptor(const Acceptor&) = delete;
        Acceptor& operator=(const Acceptor&) = delete;
};