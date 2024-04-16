#!/bin/bash
DOCKER=docker #$(docker --version 2>/dev/null >/dev/null && echo "docker" || echo "podman")
IMAGE=fuzzer
CCACHE=ccache

set -e
make clean
$DOCKER build -t $IMAGE -f fuzz/Dockerfile .
$DOCKER volume create $CCACHE --ignore
$DOCKER run --name $IMAGE --replace -v "$CCACHE:/ccache" -it $IMAGE
