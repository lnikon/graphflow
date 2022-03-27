#!/bin/bash

set -eu -o pipefail

# $1 - build directory
# $2 - conduit

export CONAN_CPU_COUNT=16
conan profile update settings.compiler.libcxx=libstdc++11 default
conan install --build missing -r conancenter --profile conanprofile.toml -if "$1" .

