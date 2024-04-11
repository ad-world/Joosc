#!/bin/bash
IMAGE=fuzzer

set -e
make clean
podman build -t $IMAGE -f fuzz/Dockerfile .
podman run --rm -it $IMAGE
