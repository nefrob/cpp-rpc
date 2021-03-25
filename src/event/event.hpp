/**
 * File: event.hpp
 * ---------------
 * Defines an event to be used with epoll-based event loops.
 */

#pragma once

/**
 * Event loop event.
 */
class Event {
    public:
        Event();

        ~Event();

    private:
        /* Set non-construction and assignment copyable. */
        Event(const Event&) = delete;
        Event& operator=(const Event&) = delete;
};