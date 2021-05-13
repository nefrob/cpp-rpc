/**
 * This client is for testing purposes only right now.
 * 
 * Client uses blocking sockets for send/recv/connect.
 */

#include <iostream>
#include <google/protobuf/stubs/common.h>
#include "utils/debug.hpp"
#include "utils/utils.hpp"
#include "utils/network_utils.hpp"
#include "rpc/message.hpp"
#include "client/client_socket.hpp"
#include "../build/src/protos/rpc.pb.h"
#include "google/protobuf/any.pb.h"

/* Static forward declarations. */
void send_echo_request(int sockfd, std::string& input);
std::string get_response(int sockfd);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Client usage: %s hostname port\n", argv[0]);
        exit(1);
    }

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    LOG_DEBUG("Starting client ...");

    std::string hostname = std::string(argv[1]);
    unsigned short port = atoi(argv[2]);

    int client_sock = create_client_socket(hostname.c_str(), port);
    if (client_sock == CLIENT_SOCK_ERROR) {
        LOG_ERROR("cannot connect to server %s:%hu", hostname.c_str(), port);
        return -1;
    }

    assert(connection_alive(client_sock));

    while (true) {
        std::string input;

        printf("client >");
        std::getline(std::cin, input);

        if (input == "s") {
            // shutdown(client_sock, SHUT_RDWR); // for error?
            close_log_err(client_sock);
            break;
        } else if (input == "e") {
            printf("echo input >");
            std::getline(std::cin, input);

            send_echo_request(client_sock, input);
        } else {
            LOG_DEBUG("Unhandled input.");
            continue;
        }
    
        printf("response: %s\n", get_response(client_sock).c_str());
    }

    LOG_DEBUG("Stopping client ...");

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}

void send_echo_request(int sockfd, std::string& input) {
    struct message msg;
    EchoRequest request;
    google::protobuf::Any pack;

    request.set_message(input);
    pack.PackFrom(request);

    msg.len = pack.ByteSizeLong();
    uint8_t buf[msg.len];
    msg.data = buf;
    pack.SerializeToArray(msg.data, msg.len);

    ssize_t n = send_all(sockfd, &msg.len, sizeof(msg.len));
    assert(n == sizeof(msg.len));
    
    n = send_all(sockfd, msg.data, msg.len);
    assert(n == msg.len);
}

std::string get_response(int sockfd) {
    struct message msg;
    google::protobuf::Any any;
    EchoResponse echo;
    ErrorResponse error;

    ssize_t n = recv_all(sockfd, &msg.len, sizeof(msg.len));
    assert(n == sizeof(msg.len));

    uint8_t buf[msg.len];
    n = recv_all(sockfd, &buf, msg.len);
    assert(n == msg.len);

    any.ParseFromArray(buf, msg.len);

    if (any.UnpackTo(&echo)) {
        return echo.message();
    } else if (any.UnpackTo(&error)) {
        return error.error();
    }
        
    return "Unknown protobuf.";
}