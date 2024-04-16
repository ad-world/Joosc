#!/bin/bash
DOCKER=$(docker --version 2>/dev/null >/dev/null && echo "docker" || echo "podman")
IMAGE=fuzzer

set -e
make clean
$DOCKER build -t $IMAGE -f fuzz/Dockerfile .
$DOCKER run --rm -it $IMAGE
