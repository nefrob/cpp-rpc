/**
 * File: accept_event.hpp
 * ----------------------
 * Defines an accept socket event to be used with epoll-based
 * event loops.
 */

#pragma once
#include "event/event.hpp"

/* Forward declarations (stop cyclic dependencies). */
class EventLoop;
class RpcResponder;

/**
 * Accept socket event loop event.
 */
class Acceptor: public Event {
    public:

        /**
         * Constructs socket handler for use with epoll event loop.
         * 
         * @param loop: loop socket is associated with.
         * @param listen_sock: server socket to listen for events on.
         * @param responder: RPC async response build/queue. FIXME: name
         */
        Acceptor(EventLoop& loop, int listen_sock, RpcResponder& responder);

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
        /* Async RPC response generator provided to new
           connections. */
        RpcResponder& responder_;

        /* Set non-construction and assignment copyable. */
        Acceptor(const Acceptor&) = delete;
        Acceptor& operator=(const Acceptor&) = delete;
};