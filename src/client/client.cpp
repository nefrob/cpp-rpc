/**
 * This client is for testing purposes only right now.
 */

#include <iostream>
#include <sys/socket.h>
#include "utils/debug.hpp"
#include "utils/utils.hpp"
#include "utils/network_utils.hpp"
#include "rpc/client_socket.hpp"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Client usage: %s hostname port\n", argv[0]);
        exit(1);
    }

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
        printf("client >");
        std::getline(std::cin, request);

        if (request == "s") {
            // TODO: close connection
            break;
        }

        // Send rpc request
        assert(connection_alive(client_sock));
        ssize_t n; // = send_rpc(request, client_sock);
        assert(n == request.length());
        
        std::string response; // = recv_rpc(client_sock);
        if (response == "") {
            LOG_ERROR("rpc receive error");
            shutdown(client_sock, SHUT_RDWR);
            close_log_err(client_sock);
            break;
        }

        printf("response: %s", response);
    }

    LOG_DEBUG("Stopping client ...");

    return 0;
}