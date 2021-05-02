/**
 * File: accept_event.hpp
 * ----------------------
 * TODO:
 */

#pragma once
#include "event/event.hpp"

/* Forward declarations (stop cyclic dependencies). */
class EventLoop;

/**
 * Acceptor event loop event.
 */
class Acceptor: public Event {
    public:

        /**
         * EPOLLIN
         */
        Acceptor(EventLoop& loop, int listen_sock);

        /**
         * Deconstructor.
         */
        ~Acceptor();

        /**
         * 
         *
         * @param: epoll set event flags.
         */
        void handle_event(uint32_t events);

        /**
         * Frees timer memory (assumes allocated with "new").
         */
        void handle_deregister();

    private:
        /* Set non-construction and assignment copyable. */
        Acceptor(const Acceptor&) = delete;
        Acceptor& operator=(const Acceptor&) = delete;
};