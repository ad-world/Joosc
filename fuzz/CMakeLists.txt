cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)

### Include LPM
include (cmake/protobuf.cmake)

### Link LPM
link_libraries(
    ${Protobuf_LIBRARIES}
    ${ProtobufMutator_LIBRARIES}
    )

### Create joosc_proto lib
add_library(joosc_proto
    src/joosc_proto.pb.cc
    src/proto_to_joos.cc
    ${Protobuf_LIBRARIES}
    )
# add_dependencies(joosc_proto ${PBM_PREFIX})

# Add proto_to_joos executable
add_executable(proto_to_joos src/proto_to_joos_main.cc)
target_link_libraries(proto_to_joos
    joosc_proto
    )

macro(fuzzer name)
    include_directories(${Protobuf_INCLUDE_DIRS})
    include_directories(${ProtobufMutator_INCLUDE_DIRS})
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    include_directories(../src)

    add_executable(${name} ${name}.cc)
    set_target_properties(${name}
            PROPERTIES
            COMPILE_FLAGS "-fsanitize=fuzzer"#,address" # -fprofile-generate -ftest-coverage"
            LINK_FLAGS "-fsanitize=fuzzer")#,address") # -fprofile-generate -ftest-coverage")
    target_link_libraries(${name}
            PRIVATE
            CommonLib
            joosc_proto
            ${Protobuf_LIBRARIES}
            ${ProtobufMutator_LIBRARIES}
            # gcov
            )
endmacro()

fuzzer( fuzz_joosc )
