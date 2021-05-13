/**
 * This server is for testing purposes only right now.
 */

#include <iostream>
#include <google/protobuf/stubs/common.h>
#include "utils/debug.hpp"
#include "utils/network_utils.hpp"
#include "rpc/rpc_server.hpp"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Server usage: %s ip port\n", argv[0]);
        exit(1);
    }

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    LOG_DEBUG("Starting server ...");
    LOG_DEBUG("Server hostname is %s", get_host_name().c_str());

    std::string ip = std::string(argv[1]);
    unsigned short port = atoi(argv[2]);

    RpcServer server(port, ip);

    while (true) {
        std::string input;
        printf("server >");
        std::getline(std::cin, input);

        if (input == "s") {
            break;
        }

    }

    LOG_DEBUG("Stopping server ...");
    server.stop();

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}