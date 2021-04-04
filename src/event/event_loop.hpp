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
#include "event/timer.hpp"

/** 
 * Epoll event loop to efficiently manage file-descriptor events.
 */
class EventLoop {
    public:
        /**
         * Function to be executed within event loop.
         */
        typedef std::function<void(void)> Thunk;

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
         * Registers event with epoll and event loop. Event pointer
         * stored in event loop so its lifetime should reflect this.
         * 
         * FIXME: could use a unique pointer with move to reflect 
         * ownership, but thunks are copyable making queue addEvent funky.
         * 
         * @param event: event being registered.
         * @param events: epoll events flags to register with.
         */
        void addEvent(Event *event, uint32_t events);

        /**
         * Deregisters event from epoll and deletes event. Useful
         * when calling from an event handler.
         * 
         * @param event: event being deregistered.
         */
        void removeEvent(Event *event);

        /**
         * Re-registers event with epoll and event loop. Useful 
         * when using EPOLLONESHOT flag and re-registering from
         * an event handler.
         * 
         * @param event: event being registered.
         * @param events: epoll events flags to reregister with.
         */
        void updateEvent(Event *event, uint32_t events);

    private:
        /* Starts the event loop waiting on events. */
        void run();

        /* Returns whether the current thread is the loop thread. */
        bool inLoopThread();

        /* Runs provided thunk in event loop. If not currently
           in event loop then thunk is queued. */
        void runInLoop(Thunk thunk);

        /* Executes pending thunks in the event loop. */
        void doPendingThunks();

        /* Adds event to epoll and store pointer. */
        void epollAdd(Event *event, uint32_t events);

        /* Deletes event from epoll and deletes pointer (and so object too). */
        void epollDel(Event *event);

        /* Modifies epoll event flags. */
        void epollMod(Event *event, uint32_t events);

        /* Maximum epoll_wait event array length. */
        static const int EVENT_ARR_LEN = 16;

        /* Epoll file descriptor. */
        int epollfd_;

        /* Events managed by event loop. */
        std::unordered_set<Event *> handled_events;

        /* Pending functions to run within event loop. */
        std::queue<Thunk> pending_thunks_;

        /* Timer to force event loop thread wakeup 
           (for scheduling thunks or ensuring event loop exits). */
        Timer wakeup_timer_;

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