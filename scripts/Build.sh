#!/bin/bash

set -eu -o pipefail

if [[ -z ${UPCXX_INSTALL} ]];
then
    echo "UPCXX_INSTALL env variable should be set to proper UPCXX installation path"
    exit
fi

export THREAD_COUNT=16

cmake -S. -B./build
cmake --build ./build -j$THREAD_COUNT
