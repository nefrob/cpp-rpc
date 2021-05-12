/**
 * File: rpc_server.hpp
 * --------------------
 * Sets up RPC server to handle requests from RPC clients.
 */

#pragma once
#include <string>
#include "event/event_loop.hpp"
#include "rpc/rpc_request_handler.hpp"

/**
 * RPC server that handles incoming RPC requests, getting the
 * respective response and returning it to the client. 
 */
class RpcServer {
    public: 
        /**
         * Constructs and starts a new RPCServer listening for
         * incoming connections.
         */
        RpcServer(uint16_t port, std::string ip);
  
        /**
         * Closes connections and stops server running.
         */
        ~RpcServer();

        /**
         * Stops server running.
         */
        void stop();

    private:
        /* Server port to listen on. */
        uint16_t port_;

        /* Server IP address to bind to. */
        std::string ip_;

        /* Server event loop for network events. */
        EventLoop loop_;

        /* Server running status. */
        bool running_;

        /* Async RPC response generator and queuer. */
        RpcRequestHandler rpc_handler_;
};