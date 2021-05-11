/**
 * File: rpc_responder.hpp
 * -----------------------
 * Builds RPC responses from incoming requests asynchronously
 * and queues them for sending.
 * 
 * FIXME: could make epoll event out of this, however using thread
 * pool makes it easy for anyone to pass in an arbitrary response
 * generating function.
 */

#pragma once
#include <functional>
#include "concurrency/threadpool.hpp"
#include "rpc/message.hpp"
#include "rpc/socket_event.hpp"

/* Forward declarations (stop cyclic dependencies). */
class EventLoop;

/**
 * Response manager.
 * 
 * FIXME: maybe rename to RpcRequestHandler
 */
class RpcResponder {
    public:
        /**
         * Function to be executed on request to get response.
         */
        typedef std::function<struct message *(struct message *)> GetRpcResponse;

        /**
         * Constructs socket handler for use with epoll event loop.
         * 
         * @param loop: server event loop.
         * @param getResponse: response generator function.
         */
        RpcResponder(EventLoop& loop, GetRpcResponse getResponse);
  
        /**
         * Deconstructor, waits on pending requests to be serviced.
         */
        ~RpcResponder();

        /**
         * Waits for pending requests to complete and joins worker threads.
         * 
         * TODO: disable accepting new requests
         */
        void stop();

        /**
         * Queues an RPC request to asynchronously build a response and 
         * queue it for sending on the provided socket.
         * 
         * @param socket: socket event to queue response on.
         * @param request: incoming RPC request to service.
         */
        void scheduleRpcRequest(std::weak_ptr<Socket> socket, struct message *request);

    private:
        /* Gets RPC response from provided request and queues it on the
           socket for sending. */
        void handleRpcRequest(std::weak_ptr<Socket> socket, struct message *request);

        /* Worker thread pool size. */
        static const size_t WORKER_THREADS = 4;

        /* Epoll-based event loop to schedule message queuing for sockets. */
        EventLoop& loop_;

        /* Response generating function. */
        GetRpcResponse getResponse_;

        /* Pool to schedule async request handling. */
        ThreadPool worker_pool_;
};