/**
 * File: event.hpp
 * ---------------
 * Defines an event to be used with epoll-based event loops.
 */

#pragma once
#include <cstdint>

/* Forward declarations (stop cyclic dependencies). */
class EventLoop;

/**
 * Event loop event.
 */
class Event {
    public:
        /**
         * Constructs event handler for use with epoll event loop.
         *
         * @param loop: loop that event is associated with.
         * @param fd: file descriptor to handle.
         * @param events: event flags to manage.
         * @param close_on_destruct: whether file descriptor should be
         *     closed on Event deconstruct (else expected to be closed
         *     by caller).
         */
        Event(EventLoop& loop, int fd, uint32_t events,
            bool close_on_destruct = true);

        ~Event();

        /**
         * Abstract method called when epoll_wait returns with event
         * on fd. Called from within event loop.
         * 
         * @param events: epoll set event flags.
         */
        virtual void handle_event(uint32_t events) = 0;

    private:
        /* File descriptor being monitored. */
        const int fd_;
        
        /* Whether fd is closed on class deconstruct. */
        bool close_on_destruct_;

        /* Epoll-based event loop that fd is registered with. */
        EventLoop& loop_;

        /* Set non-construction and assignment copyable. */
        Event(const Event&) = delete;
        Event& operator=(const Event&) = delete;
};