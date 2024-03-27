project(joosc_fuzzer CXX)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FindProtobuf)
find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS joosc_proto.proto)
add_library(jooscProto ${PROTO_SRCS} ${PROTO_HDRS})

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cc") # All files in subdirectories of src
add_executable(joosc_fuzzer PRIVATE ${SOURCES} "src/proto_to_joos_main.cpp")
target_include_directories(joosc_fuzzer PRIVATE src)
# target_link_libraries(joosc_fuzzer PRIVATE jooscProto)
