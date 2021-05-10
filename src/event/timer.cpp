#include <sys/timerfd.h>
#include <sys/epoll.h>
#include "event/timer.hpp"
#include "event/event_loop.hpp"
#include "utils/debug.hpp"

/* Static forward declarations. */
static int create_timerfd();

Timer::Timer(EventLoop& loop, TimerCallback callback): 
    timer_callback_(callback),
    Event(loop, create_timerfd()) { }

Timer::~Timer() { } // handled by Event::~Event()

void Timer::schedule(uint64_t ns) {
    if (ns == 0) ns = 1; // mininum 1 nanosecond else epoll will deschedule

    struct itimerspec new_value = {0};
    new_value.it_value.tv_sec = ns / NS_PER_SEC;
    new_value.it_value.tv_nsec = ns % NS_PER_SEC;
    if (timerfd_settime(fd(), 0, &new_value, NULL) == -1)
        PANIC("timerfd_settime failed for %luns: %s", ns, strerror(errno));
}

void Timer::deschedule() {
    struct itimerspec new_value = {0};
    if (timerfd_settime(fd(), 0, &new_value, NULL) == -1)
        PANIC("timerfd_settime failed to deschedule: %s", strerror(errno));
}

void Timer::handleEvent(uint32_t events) {
    timer_callback_(events);
}

/* Creates new non-blocking timer file descriptor. */
static int create_timerfd() {
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd == -1) PANIC("timerfd_create failed: %s", strerror(errno));

    return timerfd;
}