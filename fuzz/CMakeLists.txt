# Protobuf library
include(FindProtobuf)
find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS joosc_proto.proto)
add_library(joosc_proto
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    ${PROTOBUF_LIBRARIES}
    )

macro(fuzzer name)
    include_directories(${PROTOBUF_INCLUDE_DIRS})
    add_executable(${name} ${name}.cc)
    set_target_properties(${name}
            PROPERTIES
            COMPILE_FLAGS "-fsanitize=fuzzer,address"
            LINK_FLAGS "-fsanitize=fuzzer,address")
    target_link_libraries(${name}
            PRIVATE
            CommonLib
            joosc_proto
            protobuf-mutator-libfuzzer
            ${LIB_PROTO_MUTATOR_FUZZER_LIBRARIES}
            )
    target_include_directories(${name}
            PRIVATE
            ../src
            /usr/local/include/libprotobuf-mutator
            ${CMAKE_CURRENT_BINARY_DIR}
            )
endmacro()

fuzzer( fuzz_simple )
