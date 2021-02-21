#!/bin/bash

# vertexCount=[64,  256, 1024, 2048, 4096, 8192, 16384, 33668, 67336, 134672,  269344,   538688, 1077376]
# percentages=[100, 100, 100,  50,   15,   5,    3,     1,     0.5,   0.2,     0.08,     0.03,   0.009]
# rankCount=(1 2)

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../manual_build/graph-pgas-test --vertex-count=64 --percentage=100 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../manual_build/graph-pgas-test --vertex-count=64 --percentage=100 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../manual_build/graph-pgas-test --vertex-count=256 --percentage=100 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../manual_build/graph-pgas-test --vertex-count=256 --percentage=100 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../manual_build/graph-pgas-test --vertex-count=1024 --percentage=100 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../manual_build/graph-pgas-test --vertex-count=1024 --percentage=100 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../manual_build/graph-pgas-test --vertex-count=2048 --percentage=50 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../manual_build/graph-pgas-test --vertex-count=2048 --percentage=50 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../manual_build/graph-pgas-test --vertex-count=4096 --percentage=15 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../manual_build/graph-pgas-test --vertex-count=4096 --percentage=15 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../manual_build/graph-pgas-test --vertex-count=8192 --percentage=5 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../manual_build/graph-pgas-test --vertex-count=8192 --percentage=5 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../manual_build/graph-pgas-test --vertex-count=16384 --percentage=3 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../manual_build/graph-pgas-test --vertex-count=16384 --percentage=3 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../manual_build/graph-pgas-test --vertex-count=33668 --percentage=1 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../manual_build/graph-pgas-test --vertex-count=33668 --percentage=1 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../manual_build/graph-pgas-test --vertex-count=67336 --percentage=0.5 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../manual_build/graph-pgas-test --vertex-count=67336 --percentage=0.5 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../manual_build/graph-pgas-test --vertex-count=134672 --percentage=0.02 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../manual_build/graph-pgas-test --vertex-count=134672 --percentage=0.02 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../manual_build/graph-pgas-test --vertex-count=269344 --percentage=0.08 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../manual_build/graph-pgas-test --vertex-count=269344 --percentage=0.08 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../manual_build/graph-pgas-test --vertex-count=538688 --percentage=0.03 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../manual_build/graph-pgas-test --vertex-count=538688 --percentage=0.03 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../manual_build/graph-pgas-test --vertex-count=1077376 --percentage=0.009 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../manual_build/graph-pgas-test --vertex-count=1077376 --percentage=0.009
