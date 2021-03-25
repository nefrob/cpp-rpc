#include <sys/epoll.h>
#include "event/event_loop.hpp"
#include "utils/utils.hpp"
#include "utils/debug.hpp"

/* Static forward declarations. */
static int create_epollfd();

EventLoop::EventLoop():
    epollfd_(create_epollfd()), exit_(false) {

    loop_thread_ = std::thread([this]() { run(); });
}

EventLoop::~EventLoop() {
    stop();
}

void EventLoop::run() {
    LOG_DEBUG("Starting event loop");

    while (true) {
        lock_.lock();
        if (exit_) {
            lock_.unlock();
            break;
        }
        lock_.unlock();

        LOG_DEBUG("Waiting for events ...");

        struct epoll_event events[EVENT_ARR_LEN];
        int n_events = epoll_wait(epollfd_, events, EVENT_ARR_LEN, -1);
        if (n_events == -1) {
            if (errno == EINTR) continue;
            PANIC("epoll_wait failed: %s", strerror(errno));
        }

        for (int i = 0; i < n_events; i++) {
            ((Event *) events[i].data.ptr)->handle_event(events[i].events);
        }
    }

    LOG_DEBUG("Exiting event loop");
}

void EventLoop::stop() {
    lock_.lock();
    if (exit_) {
        lock_.unlock();
        return;
    }

    LOG_DEBUG("Stopping event loop for epollfd %d", epollfd_);
    exit_ = true;
    lock_.unlock();

    loop_thread_.join();
    close_log_err(epollfd_);
}

/* Creates new epoll file descriptor. */
static int create_epollfd() {
    int epollfd = epoll_create1(EPOLL_CLOEXEC);
    if (epollfd < 0) PANIC("epollfd create failed: %s", strerror(errno));
    return epollfd;
}