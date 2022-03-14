#!/bin/bash

set -eu -o pipefail

echo "This script is intended to be run from CMake!"

# Configure & make & install.
cd ./thirdparty/papi/src
./configure --prefix=$(pwd)/install
make -j 8 && make install

# Make symlink: liblibpapi.a -> libpapi.a so that cmake can recognize the lib.
cd install/lib
ln -s libpapi.a liblibpapi.a 
