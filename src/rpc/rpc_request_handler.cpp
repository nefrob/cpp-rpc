#include <cassert>
#include "rpc/rpc_request_handler.hpp"
#include "event/event_loop.hpp"

RpcRequestHandler::RpcRequestHandler(EventLoop& loop): loop_(loop) { }

RpcRequestHandler::~RpcRequestHandler() { }

void RpcRequestHandler::handleMessage(std::weak_ptr<Socket> socket, 
    struct message *request) {
    assert(loop_.inLoopThread());
    assert(!socket.expired());
    assert(request != NULL);
    
    // FIXME: for now just echo incoming message
    struct message *response = request;

    // * If RPC response could block, ideally it should be
    // * made into an event loop event and call below code
    // * on getting the response.
    loop_.runInLoop([socket, response]() {
        if (auto sp = socket.lock()) {
            sp->queueMessage(response->data, response->len);
        }

        free(response->data);
        free(response);
    });
}