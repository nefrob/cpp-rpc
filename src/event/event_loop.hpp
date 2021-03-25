/**
 * File: event_loop.hpp
 * --------------------
 * Defines epoll based event loop.
 */

#pragma once
#include <mutex>
#include <thread>
#include "event/event.hpp"

/** 
 * Epoll event loop to efficiently manage file-descriptor events.
 */
class EventLoop {
    public:
        /**
         * Starts new event loop waiting on epoll file descriptor for events.
         */
        EventLoop();

        /**
         * Exits event loop. 
         */
        ~EventLoop();

        // TODO: event add, mod, delete

        /**
         * Stops event loop running.
         */
        void stop();

    private:
        /* Starts the event loop looping / waiting on events. */
        void run();

        /* Maximum epoll_wait event array length. */
        static const int EVENT_ARR_LEN = 16;

        /* Epoll file descriptor. */
        int epollfd_;

        // TODO: hash set/map of registered events

        /* Thread event loop runs on. */
        std::thread loop_thread_;

        /* Event loop lock for concurrent access safety. */
        std::mutex lock_;

        /* Whether event loop should exit. */
        bool exit_;

        /* Set non-construction and assignment copyable. */
        EventLoop(const EventLoop&) = delete;
        EventLoop& operator=(const EventLoop&) = delete;
};