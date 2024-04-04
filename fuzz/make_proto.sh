#!/bin/bash
pushd "$(dirname $0)"
./protoc --cpp_out=src joosc_proto.proto
popd
