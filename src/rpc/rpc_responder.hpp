/**
 * File: rpc_responder.hpp
 * -----------------------
 *
 */

#pragma once
#include <functional>
#include "concurrency/threadpool.hpp"
#include "rpc/message.hpp"
#include "rpc/socket_event.hpp"

/* Forward declarations (stop cyclic dependencies). */
class EventLoop;

/**
 * TODO
 */
class RpcResponder {
    public:
        /**
         * generate response, must free provided request
         * fixme const args?
         */
        typedef std::function<struct message *(struct message *)> GetRpcResponse;

        /**
         * 
         */
        RpcResponder(EventLoop& loop, GetRpcResponse getResponse);
  
        /**
         * 
         */
        ~RpcResponder();

        /**
         *
         */
        void scheduleRpcRequest(std::weak_ptr<Socket> socket, struct message *request);

    private:
        /* */
        void handleRpcRequest(std::weak_ptr<Socket> socket, struct message *request);

        /* */
        static const size_t WORKER_THREADS = 4;

        /* Epoll-based event loop to schedule message queuing for sockets. */
        EventLoop& loop_;

        /* */
        GetRpcResponse getResponse_;

        /* */
        ThreadPool worker_pool_;
};