#include <fstream>
#include "rpc/rpc_responder.hpp"
#include "event/event_loop.hpp"
#include "utils/debug.hpp"

RpcResponder::RpcResponder(EventLoop& loop, GetRpcResponse getResponse): 
    worker_pool_(WORKER_THREADS), loop_(loop), getResponse_(getResponse) { }

RpcResponder::~RpcResponder() { 
    stop();
}

void RpcResponder::stop() {
    // TODO: if already said stop then don't do anything

    worker_pool_.wait();
}

void RpcResponder::scheduleRpcRequest(std::weak_ptr<Socket> socket, struct message *request) {
    assert(request != nullptr);

    // TODO: is stop called then disable queueing new requests

    worker_pool_.schedule([this, socket, request]() {
        handleRpcRequest(socket, request);
    });
}

void RpcResponder::handleRpcRequest(std::weak_ptr<Socket> socket, struct message *request) {
    assert(request != nullptr);

    struct message *response = getResponse_(request);
    assert(response != nullptr);

    loop_.runInLoop([socket, response]() {
        if (auto spSocket = socket.lock()) {
            spSocket->queueMessage(response->data, response->len);
        }
        
        free(response->data);
        free(response);
    });
}