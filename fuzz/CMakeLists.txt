######################
# Libprotobuf Mutator
######################
include(ExternalProject)

if (NOT PBM_PREFIX)
  set (PBM_PREFIX protobuf_mutator)
endif()

set(PBM_PATH ${CMAKE_CURRENT_BINARY_DIR}/${PBM_PREFIX}/src/${PBM_PREFIX})
set(PBM_LIB_PATH ${PBM_PATH}-build/src/libprotobuf-mutator.a)
set(PBM_FUZZ_LIB_PATH ${PBM_PATH}-build/src/libfuzzer/libprotobuf-mutator-libfuzzer.a)

ExternalProject_Add(${PBM_PREFIX}
  PREFIX ${PBM_PREFIX}
  GIT_REPOSITORY https://github.com/google/libprotobuf-mutator.git
  GIT_TAG master
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DLIB_PROTO_MUTATOR_DOWNLOAD_PROTOBUF=ON
  CMAKE_CACHE_ARGS -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
                   -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
  BUILD_BYPRODUCTS ${PBM_LIB_PATH} ${PBM_FUZZ_LIB_PATH}
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
  )

set(ProtobufMutator_INCLUDE_DIRS ${PBM_PATH})
set(ProtobufMutator_LIBRARIES ${PBM_FUZZ_LIB_PATH} ${PBM_LIB_PATH})
######################


# Protobuf library
include(FindProtobuf)
find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS joosc_proto.proto)
add_library(joosc_proto
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    )

macro(fuzzer name)
    add_executable(${name} ${name}.cc)
    set_target_properties(${name}
            PROPERTIES
            COMPILE_FLAGS "-fsanitize=fuzzer,address"
            LINK_FLAGS "-fsanitize=fuzzer,address")
    target_link_libraries(${name}
            PRIVATE
            CommonLib
            joosc_proto
            ${Protobuf_LIBRARIES}
            ${ProtobufMutator_LIBRARIES}
            )
    target_include_directories(${name}
            PRIVATE
            ../src
            ${PROTO_HDRS}
            ${Protobuf_INCLUDE_DIRS}
            ${ProtobufMutator_INCLUDE_DIRS}
            ${CMAKE_CURRENT_BINARY_DIR}
            )
endmacro()

fuzzer( fuzz_simple )
