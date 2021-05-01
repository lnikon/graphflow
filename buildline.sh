#!/bin/bash
set -x

cd build
conan install .. --profile ../conanprofile.toml --build missing
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=YES
cp -f compile_commands.json ../

