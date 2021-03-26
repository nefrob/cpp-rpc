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

void EventLoop::addEvent(Event *event, uint32_t events) {
    assert(event != nullptr);

    if (handled_events.find(event) != handled_events.end()) {
        LOG_WARN("Event already registered for fd %d", event->fd());
        return;
    }

    LOG_DEBUG("Registering event for fd %d", event->fd());

    struct epoll_event poll_event = {0};
    poll_event.data.fd = event->fd();
    poll_event.data.ptr = event;
    poll_event.events = events;
    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, event->fd(), &poll_event) == -1) {
        PANIC("epoll_ctl failed to add fd %d: %s", 
            event->fd(), strerror(errno));
    }

    handled_events.insert(event);
}

void EventLoop::removeEvent(Event *event) {
    assert(event != nullptr);

    if (handled_events.find(event) == handled_events.end()) {
        LOG_WARN("Event not registered for fd %d", event->fd());
        return;
    }

    LOG_DEBUG("Deregistering event for fd %d", event->fd());

    if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, event->fd(), NULL) == -1) {
        PANIC("epoll_ctl failed to remove fd %d: %s", 
            event->fd(), strerror(errno));
    }

    handled_events.erase(event);
}

void EventLoop::updateEvent(Event *event, uint32_t events) {
    assert(event != nullptr);

    if (handled_events.find(event) == handled_events.end()) {
        LOG_WARN("Event not registered for fd %d", event->fd());
        return;
    }

    LOG_DEBUG("Modifying event for fd %d", event->fd());

    struct epoll_event poll_event = {0};
    poll_event.data.fd = event->fd();
    poll_event.data.ptr = event;
    poll_event.events = events;
    if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, event->fd(), &poll_event) == -1) {
        PANIC("epoll_ctl failed to modify fd %d: %s", 
            event->fd(), strerror(errno));
    }
}

/* Creates new epoll file descriptor. */
static int create_epollfd() {
    int epollfd = epoll_create1(EPOLL_CLOEXEC);
    if (epollfd < 0) PANIC("epollfd create failed: %s", strerror(errno));
    return epollfd;
}