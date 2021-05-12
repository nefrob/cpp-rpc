cmake_minimum_required(VERSION 3.16.3)

project(server)

find_package(Protobuf REQUIRED)

# src server ...
include_directories(.. . ../utils ../event ../rpc)
include_directories(${CMAKE_BINARY_DIR}/src/protos) # compiled .proto files
include_directories(${Protobuf_INCLUDE_DIRS})

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
    ../rpc/message.hpp
    ../rpc/server_socket.hpp            ../rpc/server_socket.cpp
    ../rpc/accept_event.hpp             ../rpc/accept_event.cpp
    ../rpc/socket_event.hpp             ../rpc/socket_event.cpp
    ../rpc/socket_msg_handler.hpp       
    ../rpc/rpc_request_handler.hpp      ../rpc/rpc_request_handler.cpp 
    ../rpc/rpc_server.hpp               ../rpc/rpc_server.cpp 
)

set(PROTO_LIBS
    rpc
)

add_executable(
    ${PROJECT_NAME} ${SERVER_SRC} ${UTILS_SRC} ${EVENT_SRC} ${RPC_SRC}
)

# For std::thread
target_link_libraries(
    ${PROJECT_NAME} PUBLIC pthread ${PROTO_LIBS} ${Protobuf_LIBRARIES}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)