#!/bin/bash

set -Eeu -o pipefail

RED="\e[31m"
GREEN='\e[32m'
NC='\033[0m' # No Color

# $1 - Message to be printed.
function info_log() {
    echo -e "${GREEN}INFO${NC}: $1"
}

# $1 - Message to be printed.
function error_log() {
    echo -e "${RED}ERROR${NC}: $1"
}

# Correct $UPCXX_INSTALL is mandatory for builds to puss
if [[ -z ${UPCXX_INSTALL} ]];
then
    error_log "UPCXX_INSTALL env variable should be set to proper UPCXX installation path"
    exit
fi

# $1 - Build directory.
# $2 - UPCXX conduit to be used. Possible values are: smp, udp, mpi, ibv.
function build_pgasgrah() {
    export THREAD_COUNT=16
    export UPCXX_NETWORK=$2
    info_log "Building for ${UPCXX_NETWORK} conduit"
    cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=On -DCMAKE_CXX_COMPILER=/usr/bin/g++ -S. -B"$1"
    cmake --build "$1" -j$THREAD_COUNT
}

build_pgasgrah $1 "smp"

cp -f "$1"/compile_commands.json .
