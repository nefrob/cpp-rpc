#include "event/event.hpp"
#include "event/event_loop.hpp"
#include "utils/utils.hpp"

Event::Event(EventLoop& loop, int fd, uint32_t events,
        bool close_on_destruct): 
    loop_(loop), fd_(fd), 
    close_on_destruct_(close_on_destruct) {

    // TODO: register in event loop
}

Event::~Event() {
    // TODO: deregister from event loop

    if (close_on_destruct_) close_log_err(fd_);
}