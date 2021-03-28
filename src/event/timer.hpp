/**
 * File: timer.hpp
 * ---------------
 * Defines a timer event to be used with epoll-based event loops.
 */

#pragma once
#include <cstdint>
#include <functional>
#include "event/event.hpp"

/* Forward declarations (stop cyclic dependencies). */
class EventLoop;

/**
 * Timer event loop event.
 * 
 * Note: schedule/deschedule methods are not thread safe
 * (ex. typically accessed under event loop lock).
 */
class Timer: public Event {
    public:
        /**
         * Callback function to be executed on timer event trigger.
         */
        typedef std::function<void(uint32_t)> TimerCallback;

        /**
         * Constructs timer handler for use with epoll event loop.
         *
         * @param loop: loop event is handling timer events from.
         * @param callback: function called when timer event triggers.
         */
        Timer(EventLoop& loop, TimerCallback callback);

        /**
         * Deconstructor.
         */
        ~Timer();

        /**
         * Schedules relative timer event to trigger.
         * 
         * @param ns: nanoseconds after which timer is triggered.
         */
        void schedule(uint64_t ns);

        /**
         * Deschedules timer event (if any) is set.
         */
        void deschedule();

        /**
         * Calls timer_callback on epoll event trigger (can be
         * due to timer or error).
         *
         * @param: epoll set event flags.
         */
        void handle_event(uint32_t events);

    private:
        /* Nanoseconds in a second. */
        static const uint64_t NS_PER_SEC = 1000000000;

        /* Timer event callback function. */
        TimerCallback timer_callback_;

        /* Set non-construction and assignment copyable. */
        Timer(const Timer&) = delete;
        Timer& operator=(const Timer&) = delete;
};