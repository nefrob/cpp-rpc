#include "event/event.hpp"
#include "event/event_loop.hpp"
#include "utils/utils.hpp"

Event::Event(EventLoop& loop, int fd, bool close_on_destruct): 
    loop_(loop), fd_(fd), 
    close_on_destruct_(close_on_destruct) {
}

Event::~Event() {
    if (close_on_destruct_) close_log_err(fd_);
}

int Event::fd() {
    return fd_;
}