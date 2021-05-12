cmake_minimum_required(VERSION 3.16.3)

project(Proto)

find_package(Protobuf REQUIRED)

set(PROTO_SRC
    rpc.proto
)

# Generate header and source files for all the proto files
foreach(proto ${PROTO_SRC})
    # Filename without extension
    get_filename_component(PROTO_NAME_WE ${proto} NAME_WE)
    get_filename_component(PROTO_NAME ${proto} NAME)
    
    message(STATUS "Compiling protobuf '${PROTO_NAME}' to C++ output")

    # Generate .cc and .h files
    protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${proto})
    
    add_library(${PROTO_NAME_WE} STATIC ${PROTO_SRCS} ${PROTO_HDRS})
    target_include_directories(${PROTO_NAME_WE} PUBLIC ${Protobuf_INCLUDE_DIRS})
endforeach()