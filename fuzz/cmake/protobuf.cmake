set(CMAKE_CXX_STANDARD 17)
include(ExternalProject)

if (NOT PBM_PREFIX)
  set (PBM_PREFIX protobuf_mutator)
endif()

set(PBM_PATH /${PBM_PREFIX})
set(PBM_LIB_PATH ${PBM_PATH}/build/src/libprotobuf-mutator.a)
set(PBM_FUZZ_LIB_PATH ${PBM_PATH}/build/src/libfuzzer/libprotobuf-mutator-libfuzzer.a)

set(PB_PATH ${PBM_PATH}/build/external.protobuf)
set(PB_LIB_PATH ${PB_PATH}/lib/libprotobuf.a)

# ExternalProject_Add(${PBM_PREFIX}
#   PREFIX ${PBM_PREFIX}
#   GIT_REPOSITORY https://github.com/google/libprotobuf-mutator.git
#   GIT_TAG af3bb18 #c390388
#   CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DLIB_PROTO_MUTATOR_DOWNLOAD_PROTOBUF=ON
#   CMAKE_CACHE_ARGS -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
#                    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
#                    -DCMAKE_CXX_STANDARD=17
#   BUILD_BYPRODUCTS ${PBM_LIB_PATH} ${PBM_FUZZ_LIB_PATH} ${PB_LIB_PATH}
#   UPDATE_COMMAND ""
#   INSTALL_COMMAND ""
#   )

set(ProtobufMutator_INCLUDE_DIRS ${PBM_PATH})
set(ProtobufMutator_LIBRARIES ${PBM_FUZZ_LIB_PATH} ${PBM_LIB_PATH})

set(Protobuf_LIBRARIES ${PB_LIB_PATH})
set(Protobuf_INCLUDE_DIRS ${PB_PATH}/include)
