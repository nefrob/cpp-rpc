cmake_minimum_required(VERSION 3.16.3)

project(server)

# src server ...
include_directories(.. .)

# Project files

set(SERVER_SRC
    server.cpp
)

add_executable(${PROJECT_NAME} ${SERVER_SRC})

# For std::thread
target_link_libraries(${PROJECT_NAME} pthread)

set_target_properties(${PROJECT_NAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)