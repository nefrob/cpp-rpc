/**
 * File: rpc_request_handler.hpp
 * -----------------------------
 * Handles incoming RPC requests on event loop sockets.
 */

#pragma once
#include <memory>
#include "rpc/message.hpp"
#include "rpc/socket_event.hpp"
#include "rpc/socket_msg_handler.hpp"

/* Forward declarations (stop cyclic dependencies). */
class EventLoop;

/**
 * RPC request handler.
 */
class RpcRequestHandler: public SocketMessageHandler {
    public:
        /**
         * Constructs request handler.
         * 
         * @param loop: server event loop.
         */
        RpcRequestHandler(EventLoop& loop);

        /**
         * Deconstructor.
         */
        ~RpcRequestHandler();

        /**
         * Gets response to incoming request and queues 
         * response on socket.
         * 
         * @param socket: socket event to queue response on.
         * @param request: incoming request to service.
         */
        void handleMessage(std::weak_ptr<Socket> socket, 
            struct message *request);

    private:
        /* Loop to schedule message queuing for sockets. */
        EventLoop& loop_;
};