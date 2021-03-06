cmake_minimum_required(VERSION 3.16.3)

project(client)

find_package(Protobuf REQUIRED)

# src client ...
include_directories(.. . ../utils ../event ../rpc)
include_directories(${CMAKE_BINARY_DIR}/src/protos) # compiled .proto files
include_directories(${Protobuf_INCLUDE_DIRS})

# Project files

set(CLIENT_SRC
    client.cpp
    client_socket.hpp                   client_socket.cpp
)

set(UTILS_SRC
    ../utils/debug.hpp                  ../utils/debug.cpp
    ../utils/utils.hpp                  ../utils/utils.cpp
    ../utils/network_utils.hpp          ../utils/network_utils.cpp
)

set(RPC_SRC
    ../rpc/message.hpp
)

set(PROTO_LIBS
    rpc
)

add_executable(${PROJECT_NAME} ${CLIENT_SRC} ${UTILS_SRC} ${RPC_SRC})

# For std::thread
target_link_libraries(
    ${PROJECT_NAME} PUBLIC pthread ${PROTO_LIBS} ${Protobuf_LIBRARIES}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)