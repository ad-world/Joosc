#!/bin/bash
IMAGE_NAME="cs444"

{
set -e
docker build -t $IMAGE_NAME .
docker run --rm -it \
    -v "$PWD:/myapp" \
    $IMAGE_NAME bash
}
