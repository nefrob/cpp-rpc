/**
 * File: event_loop.hpp
 * --------------------
 * Defines epoll based event loop.
 */

#pragma once

/** 
 * Epoll event loop to efficiently manage file-descriptor events.
 */
class EventLoop {
    public:
        EventLoop();

        ~EventLoop();

    private:
        /* Set non-construction and assignment copyable. */
        EventLoop(const EventLoop&) = delete;
        EventLoop& operator=(const EventLoop&) = delete;
};