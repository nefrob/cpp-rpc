cmake_minimum_required(VERSION 3.16.3)

project(server)

# src server ...
include_directories(.. . ../utils ../event)

# Project files

set(SERVER_SRC
    server.cpp
)

set(UTILS_SRC
    ../utils/debug.hpp                  ../utils/debug.cpp
    ../utils/utils.hpp                  ../utils/utils.cpp
    ../utils/network_utils.hpp          ../utils/network_utils.cpp
)

set(EVENT_SRC
    ../event/event.hpp                  ../event/event.cpp
    ../event/event_loop.hpp             ../event/event_loop.cpp
    ../event/timer.hpp                  ../event/timer.cpp
)

set(RPC_SRC
    ../rpc/server_socket.hpp            ../rpc/server_socket.cpp
    ../rpc/rpc_server.hpp               ../rpc/rpc_server.cpp 
)

add_executable(${PROJECT_NAME} ${SERVER_SRC} ${UTILS_SRC} ${EVENT_SRC} ${RPC_SRC})

# For std::thread
target_link_libraries(${PROJECT_NAME} pthread)

set_target_properties(${PROJECT_NAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)