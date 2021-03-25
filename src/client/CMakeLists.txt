cmake_minimum_required(VERSION 3.16.3)

project(client)

# src client ...
include_directories(.. . ../utils ../event)

# Project files

set(CLIENT_SRC
    client.cpp
)

set(UTILS_SRC
    ../utils/debug.hpp                  ../utils/debug.cpp
    ../utils/utils.hpp                  ../utils/utils.cpp
)

set(EVENT_SRC
    ../event/event.hpp                  ../event/event.cpp
    ../event/event_loop.hpp             ../event/event_loop.cpp
)

add_executable(${PROJECT_NAME} ${CLIENT_SRC} ${UTILS_SRC} ${EVENT_SRC})

# For std::thread
target_link_libraries(${PROJECT_NAME} pthread)

set_target_properties(${PROJECT_NAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)