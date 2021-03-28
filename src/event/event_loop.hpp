/**
 * File: event_loop.hpp
 * --------------------
 * Defines epoll based event loop.
 */

#pragma once
#include <mutex>
#include <thread>
#include <unordered_set>
#include <queue>
#include <functional>
#include "event/event.hpp"

/** 
 * Epoll event loop to efficiently manage file-descriptor events.
 */
class EventLoop {
    public:
        /**
         * Function to be executed within event loop.
         */
        typedef std::function<void(void)> thunk;

        /**
         * Starts new event loop waiting on epoll file descriptor for events.
         */
        EventLoop();

        /**
         * Exits event loop. 
         */
        ~EventLoop();

        /**
         * Stops event loop running.
         */
        void stop();

        /**
         * Registers event with epoll and stores pointer in event loop.
         * 
         * @param event: event being registered. Should be allocated with
         *     "new" as event loop takes ownership.
         * @param events: epoll events flags to register with.
         */
        void addEvent(Event *event, uint32_t events);

        /**
         * Deregisters event from epoll and deletes pointer from the
         * event loop.
         * 
         * @param event: event being deregistered.
         */
        void removeEvent(Event *event);

        /**
         * Re-registers event with epoll and event loop. Useful 
         * when using EPOLLONESHOT flag.
         * 
         * @param event: event being registered.
         * @param events: epoll events flags to reregister with.
         */
        void updateEvent(Event *event, uint32_t events);

        /**
         * Runs provided thunk in event loop. If not currently
         * in event loop then thunk is queued.
         * 
         * @param thunk: thunk to execute.
         */
        void runInLoop(thunk thunk);

    private:
        /* Starts the event loop waiting on events. */
        void run();

        /* Returns whether the current thread is the loop thread. */
        bool inLoopThread();

        /* Queues a thunk to be executed in the event loop. */
        void queueInLoop(thunk thunk);

        /* Executes pending thunks in the event loop. */
        void doPendingThunks();

        /* Maximum epoll_wait event array length. */
        static const int EVENT_ARR_LEN = 16;

        /* Epoll file descriptor. */
        int epollfd_;

        /* Events managed by event loop. */
        std::unordered_set<Event *> handled_events;

        /* Pending functions to run within event loop. */
        std::queue<thunk> pending_thunks_;

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