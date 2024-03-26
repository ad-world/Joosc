#!/bin/bash

docker --version
RC=$?
RUNNER="docker"
if (( $RC != 0 )); then
    RUNNER="podman"
fi

$RUNNER run --rm -it -v "../../:/src:z" aflplusplus/aflplusplus sh -c 'cd ../src && make -C tests/fuzzing clean collect-inputs all && afl-fuzz -i tests/fuzzing/INPUTS -o tests/fuzzing/OUTPUTS -- ./joosc @@ $(find tests/stdlib/java/ -type f)'
