#!/bin/bash
set -eux -o pipefail

mkdir build && cd build

conan profile new default --detect
conan profile update settings.compiler.libcxx=libstdc++11 default
conan install .. --profile ../conanprofile.toml --build missing

cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=YES -DCMAKE_CXX_COMPILER=/usr/bin/mpiCC

cp -f compile_commands.json ../

make -j2
