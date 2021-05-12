/**
 * File: socket_msg_handler.hpp
 * ----------------------------
 * Abstract class to manage incoming messages on socket events.
 */

#pragma once
#include <memory>
#include "rpc/message.hpp"
#include "rpc/socket_event.hpp"

/**
 * Request handler.
 */
class SocketMessageHandler {
    public:
        /**
         * Abstract method called by socket events after reading a
         * new message (or request). Socket call handleMessage from 
         * the event loop thread so it should not block.
         * 
         * @param socket: socket event to queue response on.
         * @param msg: incoming message to service.
         */
        virtual void handleMessage(std::weak_ptr<Socket> socket, 
            struct message *msg) = 0;
};