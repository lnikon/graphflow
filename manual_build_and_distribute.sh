#!/bin/bash

set -x

mkdir manual_build
cd manual_build
/home/ubuntu/mst/libs/upcxx-install/bin/upcxx -codemode=debug -threadmode=seq -network=udp -std=c++17 -O -I../inc  ../src/main.cpp ../src/graph-utilities.cpp ../src/pgas-graph.cpp -lboost_program_options -lboost_timer -o graph-pgas-test
scp graph-pgas-test ubuntu@185.127.66.89:~/mst/pgas-graph/manual_build
