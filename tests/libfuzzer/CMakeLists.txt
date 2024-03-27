cmake_minimum_required(VERSION 3.16)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
project(joosc_fuzzer CXX)

# Protobuf library
include(FindProtobuf)
find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS joosc_proto.proto)
add_library(joosc-proto
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    ${PROTOBUF_LIBRARIES}
    )

# Build and include libprotobuf-mutator
# include(ProtobufMutator)
# include_directories(${ProtobufMutator_INCLUDE_DIRS})

# Executable
include_directories(${Protobuf_INCLUDE_DIRS})
add_executable(joosc_fuzzer joosc_fuzzer.cc)
add_compile_options(-g -O1 -fsanitize=fuzzer)
target_link_libraries(joosc_fuzzer
    joosc-proto
    )

# file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cc") # All files in subdirectories of src
# add_executable(joosc_fuzzer PRIVATE ${SOURCES} "src/proto_to_joos_main.cpp")
# target_include_directories(joosc_fuzzer PRIVATE src)
# target_link_libraries(joosc_fuzzer PRIVATE jooscProto)
