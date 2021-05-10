/**
 * File: accept_event.hpp
 * ----------------------
 * TODO:
 */

#pragma once
#include "event/event.hpp"

/* Forward declarations (stop cyclic dependencies). */
class EventLoop;
class RpcResponder;

/**
 * Acceptor event loop event.
 */
class Acceptor: public Event {
    public:

        /**
         * EPOLLIN
         */
        Acceptor(EventLoop& loop, int listen_sock, RpcResponder& responder);

        /**
         * Deconstructor.
         */
        ~Acceptor();

        /**
         * 
         *
         * @param: epoll set event flags.
         */
        void handleEvent(uint32_t events);

    private:
        /* */
        RpcResponder& responder_;

        /* Set non-construction and assignment copyable. */
        Acceptor(const Acceptor&) = delete;
        Acceptor& operator=(const Acceptor&) = delete;
};