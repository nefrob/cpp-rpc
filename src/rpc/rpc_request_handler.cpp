#include <cassert>
#include "rpc/rpc_request_handler.hpp"
#include "../build/src/protos/rpc.pb.h"
#include "google/protobuf/any.pb.h"
#include "event/event_loop.hpp"
#include "utils/debug.hpp"

/* Static forward declarations. */
static struct message *get_response(struct message *request);
static struct message *get_echo_response(EchoRequest& request);
static struct message *get_error_response();

RpcRequestHandler::RpcRequestHandler(EventLoop& loop): loop_(loop) { }

RpcRequestHandler::~RpcRequestHandler() { }

void RpcRequestHandler::handleMessage(std::weak_ptr<Socket> socket, 
    struct message *request) {
    assert(loop_.inLoopThread());
    assert(!socket.expired());
    assert(request != NULL);
    
    struct message *response = get_response(request);
    
    free(request->data);
    free(request);

    // * If get_response could block, ideally it should
    // * should use the event loop to get the response.
    // * The below code is how you would queue the resulting
    // * response on the socket. Here we are already in the
    // * event loop so runInLoop is redundant.
    loop_.runInLoop([socket, response]() {
        if (auto sp = socket.lock()) {
            sp->queueMessage(response->data, response->len);
        }

        free(response->data);
        free(response);
    });
}

static struct message *get_response(struct message *request) {
    assert(request != NULL);
    assert(request->data != NULL);

    google::protobuf::Any any;
    EchoRequest echoReq;

    any.ParseFromArray(request->data, request->len);

    if (any.UnpackTo(&echoReq)) {
        return get_echo_response(echoReq);
    }

    return get_error_response();
}

static struct message *get_echo_response(EchoRequest& request) {
    EchoResponse response;
    google::protobuf::Any pack;
    struct message *msg;

    response.set_message(request.message());
    pack.PackFrom(response);

    msg = (struct message *) malloc(sizeof(struct message));
    if (msg == NULL) PANIC("malloc fail");

    msg->len = pack.ByteSizeLong();
    msg->data = malloc(msg->len);
    if (msg->data == NULL) PANIC("malloc fail");

    pack.SerializeToArray(msg->data, msg->len);

    return msg;
}

static struct message *get_error_response() {
    ErrorResponse response;
    google::protobuf::Any pack;
    struct message *msg;

    response.set_error("Invalid Request.");
    pack.PackFrom(response);

    msg = (struct message *) malloc(sizeof(struct message));
    if (msg == NULL) PANIC("malloc fail");

    msg->len = pack.ByteSizeLong();
    msg->data = malloc(msg->len);
    if (msg->data == NULL) PANIC("malloc fail");

    pack.SerializeToArray(msg->data, msg->len);

    return msg;
}