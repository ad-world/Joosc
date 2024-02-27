#!/bin/bash

if [ $0 == $BASH_SOURCE ]; then
    echo "Usage: source $BASH_SOURCE"
    exit 1
fi

export STDLIB="$(find tests/stdlib/java/ -name '*.java')"
