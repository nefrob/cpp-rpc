/**
 * This server is for testing purposes only right now.
 */

#include <iostream>
#include <unistd.h>
#include <google/protobuf/stubs/common.h>
#include "utils/debug.hpp"
#include "utils/network_utils.hpp"
#include "rpc/rpc_server.hpp"

/* Static forward declarations. */
static void print_usage_help();

int main(int argc, char *argv[]) {
    int opt;
    std::string ip = "";
    uint16_t port = 0;
    std::string log_arg = "SILENT";
    while ((opt = getopt(argc, argv, "i:p:vh")) != -1) {
        switch (opt) {
            case 'i':
                ip = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'v':
                log_arg = "DEBUG";
                break;
            case 'h':
                print_usage_help();
                break;
            case '?':
                printf("unknown option %c\n", optopt);
                print_usage_help();
                exit(1);
        }
    }

    if (ip == "" || port == 0) {
        printf("Error:\n");
        exit(1);
    }

    if (log_arg == "DEBUG") {
        set_log_level(LogLevel::DEBUG);
    } else {
        set_log_level(LogLevel::SILENT);
    }

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    LOG_DEBUG("Starting server ...");
    LOG_DEBUG("Server hostname is %s", get_host_name().c_str());

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

static void print_usage_help() {
    printf("Usage: -i ip_address -p port -v -h\n");
    printf("ex. -i 127.0.0.1 -p 2000\n");
    printf("-v sets verbose, -h shows this help message");
    printf("An ip address and port must be specified");
}