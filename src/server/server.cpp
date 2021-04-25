/**
 * This server is for testing purposes only right now.
 */

#include <sys/epoll.h>
#include <iostream>
#include "utils/debug.hpp"
#include "event/event_loop.hpp"
#include "event/timer.hpp"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Server usage: %s ip port\n", argv[0]);
        exit(1);
    }

    LOG_DEBUG("Starting server ...");

    std::string ip = std::string(argv[1]);
    unsigned short port = atoi(argv[2]);

    EventLoop loop;
    Timer *timer = new Timer(loop, [](uint32_t) {
        LOG_DEBUG("Test timer triggered.");
    });

    loop.addEvent(timer, EPOLLIN | EPOLLONESHOT);

    while (true) {
        std::string request;
        printf("server >");
        std::getline(std::cin, request);

        if (request == "s") {
            loop.removeEvent(timer);
            break;
        } else if (request == "r") {
            loop.updateEvent(timer, EPOLLIN | EPOLLONESHOT);
            timer->schedule(2000000000); // schedule 2 second timer
        } else if (request == "d") {
            loop.removeEvent(timer);
        }
    }

    LOG_DEBUG("Stopping server ...");
    loop.stop();

    return 0;
}