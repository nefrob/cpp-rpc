cmake_minimum_required(VERSION 3.16.3)

project(RPC VERSION 0.1.0)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

add_subdirectory(src/protos)
add_subdirectory(src/server)
add_subdirectory(src/client)