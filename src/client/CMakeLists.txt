cmake_minimum_required(VERSION 3.16.3)

project(client)

# src client ...
include_directories(.. .)

# Project files

set(CLIENT_SRC
    client.cpp
)

add_executable(${PROJECT_NAME} ${CLIENT_SRC})

set_target_properties(${PROJECT_NAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)