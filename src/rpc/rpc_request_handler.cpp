#include <cassert>
#include "rpc/rpc_request_handler.hpp"

RpcRequestHandler::RpcRequestHandler() { }

RpcRequestHandler::~RpcRequestHandler() { }

void RpcRequestHandler::handleMessage(std::weak_ptr<Socket> socket, 
    struct message *request) {
    assert(!socket.expired());
    assert(request != NULL);
    
    // FIXME: for now just echo incoming message
    struct message *response = request;

    // TODO:

    // loop_.runInLoop([]() {
    //     auto sp = socket.lock()
    //     queueMessage(response->data, response->len);

    //     free(response->data);
    //     free(response);
    // });
}