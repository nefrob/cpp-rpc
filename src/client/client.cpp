/**
 * This client is for testing purposes only right now.
 * 
 * Client uses blocking sockets for send/recv/connect.
 */

#include <iostream>
#include <google/protobuf/stubs/common.h>
#include <sys/socket.h>
#include "utils/debug.hpp"
#include "utils/utils.hpp"
#include "utils/network_utils.hpp"
#include "rpc/message.hpp"
#include "client/client_socket.hpp"

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

    while (true) {
        std::string request;
        struct message msg;

        printf("client >");
        std::getline(std::cin, request);

        if (request == "s") {
            // shutdown(client_sock, SHUT_RDWR); // for error?
            close_log_err(client_sock);
            break;
        }

        assert(connection_alive(client_sock));

        msg.len = request.length() + 1;
        msg.data = (void *) request.c_str();

        ssize_t n = send_all(client_sock, &msg.len, sizeof(msg.len));
        assert(n == sizeof(msg.len));
        
        n = send_all(client_sock, msg.data, msg.len);
        assert(n == msg.len);
        
        n = recv_all(client_sock, &msg.len, sizeof(msg.len));
        assert(n == sizeof(msg.len));

        uint8_t response[msg.len];
        n = recv_all(client_sock, &response, msg.len);
        assert(n == msg.len);

        LOG_DEBUG("response: %s", (char *) response);
    }

    LOG_DEBUG("Stopping client ...");

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}