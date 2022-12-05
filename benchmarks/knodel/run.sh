#!/bin/bash

set -x

# vertexCount=[64,  256, 1024, 2048, 4096, 8192, 16384, 33668, 67336, 134672,  269344,   538688, 1077376]
# percentages=[100, 100, 100,  50,   15,   5,    3,     1,     0.5,   0.2,     0.08,     0.03,   0.009]
# rankCount=(1 2)

# $UPCXX_INSTALL/bin/upcxx-run -n 1 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=64      --percentage=100 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 2 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=64      --percentage=100 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 1 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=256     --percentage=100 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 2 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=256     --percentage=100 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 1 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=1024    --percentage=100 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 2 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=1024    --percentage=100 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 1 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=2048    --percentage=50 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 2 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=2048    --percentage=50 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 1 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=4096    --percentage=50 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 2 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=4096    --percentage=50 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 1 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=8192    --percentage=50 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 2 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=8192    --percentage=50 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 1 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=16384   --percentage=10 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 2 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=16384   --percentage=10 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 1 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=33668   --percentage=1 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 2 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=33668   --percentage=1 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 1 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=67336   --percentage=0.5 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 2 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=67336   --percentage=0.5 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 1 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=134672  --percentage=0.05 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 2 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=134672  --percentage=0.05 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 1 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=269344  --percentage=0.03 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 2 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=269344  --percentage=0.03 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 1 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=538688  --percentage=0.02 &&
# $UPCXX_INSTALL/bin/upcxx-run -n 2 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=538688  --percentage=0.02
#$UPCXX_INSTALL/bin/upcxx-run -n 1 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=1077376 --percentage=0.125 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=1077376 --percentage=0.125

#!/bin/bash

# vertexCount=[64,  256, 1024, 2048, 4096, 8192, 10000, 33668, 67336, 134672,  269344,   538688, 1077376]
# percentages=[100, 100, 100,  50,   15,   5,    3,     1,     0.5,   0.2,     0.08,     0.03,   0.009]
# rankCount=(1 2)

export PGAS_BINARY_PATH="./../../build/src/PGASGraphCLI/pgas-graph-cli-smp"
#../../build/src/PGASGraphCLI/pgas-graph-cli-smp --vertex-count=128 --percentage=5 --model=knodel

$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=5  --model=uniform --metrics-json-path=nodes-1-model-uniform-vertexNumber-10000-percentage-5-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=5  --model=uniform --metrics-json-path=nodes-2-model-uniform-vertexNumber-10000-percentage-5-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=5  --model=uniform --metrics-json-path=nodes-4-model-uniform-vertexNumber-10000-percentage-5-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=5  --model=uniform --metrics-json-path=nodes-8-model-uniform-vertexNumber-10000-percentage-5-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 16  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=5  --model=uniform --metrics-json-path=nodes-16-model-uniform-vertexNumber-10000-percentage-5-algorithm-randomizedpushgossip.json &&

$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=10 --model=uniform --metrics-json-path=nodes-1-model-uniform-vertexNumber-10000-percentage-10-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=10 --model=uniform --metrics-json-path=nodes-2-model-uniform-vertexNumber-10000-percentage-10-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=10 --model=uniform --metrics-json-path=nodes-4-model-uniform-vertexNumber-10000-percentage-10-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=10 --model=uniform --metrics-json-path=nodes-8-model-uniform-vertexNumber-10000-percentage-10-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 16  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=10 --model=uniform --metrics-json-path=nodes-16-model-uniform-vertexNumber-10000-percentage-10-algorithm-randomizedpushgossip.json &&

$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=15 --model=uniform --metrics-json-path=nodes-1-model-uniform-vertexNumber-10000-percentage-15-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=15 --model=uniform --metrics-json-path=nodes-2-model-uniform-vertexNumber-10000-percentage-15-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=15 --model=uniform --metrics-json-path=nodes-4-model-uniform-vertexNumber-10000-percentage-15-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=15 --model=uniform --metrics-json-path=nodes-8-model-uniform-vertexNumber-10000-percentage-15-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 16  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=15 --model=uniform --metrics-json-path=nodes-16-model-uniform-vertexNumber-10000-percentage-15-algorithm-randomizedpushgossip.json &&

$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=20 --model=uniform --metrics-json-path=nodes-1-model-uniform-vertexNumber-10000-percentage-20-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=20 --model=uniform --metrics-json-path=nodes-2-model-uniform-vertexNumber-10000-percentage-20-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=20 --model=uniform --metrics-json-path=nodes-4-model-uniform-vertexNumber-10000-percentage-20-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=20 --model=uniform --metrics-json-path=nodes-8-model-uniform-vertexNumber-10000-percentage-20-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 16  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=20 --model=uniform --metrics-json-path=nodes-16-model-uniform-vertexNumber-10000-percentage-20-algorithm-randomizedpushgossip.json &&

$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=25 --model=uniform --metrics-json-path=nodes-1-model-uniform-vertexNumber-10000-percentage-25-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=25 --model=uniform --metrics-json-path=nodes-2-model-uniform-vertexNumber-10000-percentage-25-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=25 --model=uniform --metrics-json-path=nodes-4-model-uniform-vertexNumber-10000-percentage-25-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=25 --model=uniform --metrics-json-path=nodes-8-model-uniform-vertexNumber-10000-percentage-25-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 16  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=25 --model=uniform --metrics-json-path=nodes-16-model-uniform-vertexNumber-10000-percentage-25-algorithm-randomizedpushgossip.json &&

$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=5  --model=knodel --metrics-json-path=nodes-1-model-knodel-vertexNumber-10000-percentage-5-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=5  --model=knodel --metrics-json-path=nodes-2-model-knodel-vertexNumber-10000-percentage-5-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=5  --model=knodel --metrics-json-path=nodes-4-model-knodel-vertexNumber-10000-percentage-5-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=5  --model=knodel --metrics-json-path=nodes-8-model-knodel-vertexNumber-10000-percentage-5-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 16  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=5  --model=knodel --metrics-json-path=nodes-16-model-knodel-vertexNumber-10000-percentage-5-algorithm-randomizedpushgossip.json &&

$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=10 --model=knodel --metrics-json-path=nodes-1-model-knodel-vertexNumber-10000-percentage-10-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=10 --model=knodel --metrics-json-path=nodes-2-model-knodel-vertexNumber-10000-percentage-10-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=10 --model=knodel --metrics-json-path=nodes-4-model-knodel-vertexNumber-10000-percentage-10-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=10 --model=knodel --metrics-json-path=nodes-8-model-knodel-vertexNumber-10000-percentage-10-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 16  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=10 --model=knodel --metrics-json-path=nodes-16-model-knodel-vertexNumber-10000-percentage-10-algorithm-randomizedpushgossip.json &&

$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=15 --model=knodel --metrics-json-path=nodes-1-model-knodel-vertexNumber-10000-percentage-15-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=15 --model=knodel --metrics-json-path=nodes-2-model-knodel-vertexNumber-10000-percentage-15-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=15 --model=knodel --metrics-json-path=nodes-4-model-knodel-vertexNumber-10000-percentage-15-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=15 --model=knodel --metrics-json-path=nodes-8-model-knodel-vertexNumber-10000-percentage-15-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 16  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=15 --model=knodel --metrics-json-path=nodes-16-model-knodel-vertexNumber-10000-percentage-15-algorithm-randomizedpushgossip.json &&

$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=20 --model=knodel --metrics-json-path=nodes-1-model-knodel-vertexNumber-10000-percentage-20-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=20 --model=knodel --metrics-json-path=nodes-2-model-knodel-vertexNumber-10000-percentage-20-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=20 --model=knodel --metrics-json-path=nodes-4-model-knodel-vertexNumber-10000-percentage-20-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=20 --model=knodel --metrics-json-path=nodes-8-model-knodel-vertexNumber-10000-percentage-20-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 16  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=20 --model=knodel --metrics-json-path=nodes-16-model-knodel-vertexNumber-10000-percentage-20-algorithm-randomizedpushgossip.json &&

$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=25 --model=knodel --metrics-json-path=nodes-1-model-knodel-vertexNumber-10000-percentage-25-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=25 --model=knodel --metrics-json-path=nodes-2-model-knodel-vertexNumber-10000-percentage-25-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=25 --model=knodel --metrics-json-path=nodes-4-model-knodel-vertexNumber-10000-percentage-25-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000  --percentage=25 --model=knodel --metrics-json-path=nodes-8-model-knodel-vertexNumber-10000-percentage-25-algorithm-randomizedpushgossip.json &&
$UPCXX_INSTALL/bin/upcxx-run -n 16  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=25 --model=knodel --metrics-json-path=nodes-16-model-knodel-vertexNumber-10000-percentage-25-algorithm-randomizedpushgossip.json


#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=5 --model=knodel --metrics-json-path=nodes-1-model-knodel-vertexNumber-10000-percentage-5-algorithm-randomizedpushgossip.json &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=10 --model=knodel --metrics-json-path=nodes-2-model-knodel-vertexNumber-10000-percentage-10-algorithm-randomizedpushgossip.json &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=15 --model=knodel --metrics-json-path=nodes-4-model-knodel-vertexNumber-10000-percentage-15-algorithm-randomizedpushgossip.json &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=20 --model=knodel --metrics-json-path=nodes-8-model-knodel-vertexNumber-10000-percentage-20-algorithm-randomizedpushgossip.json &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=25 --model=knodel --metrics-json-path=nodes-16-model-knodel-vertexNumber-10000-percentage-25-algorithm-randomizedpushgossip.json &&
#
#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=5   --model=uniform --metrics-json-path=nodes-1-model-uniform-vertexNumber-10000-percentage-5-algorithm-randomizedpushgossip.json &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=10  --model=uniform --metrics-json-path=nodes-2-model-uniform-vertexNumber-10000-percentage-10-algorithm-randomizedpushgossip.json &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=15  --model=uniform --metrics-json-path=nodes-4-model-uniform-vertexNumber-10000-percentage-15-algorithm-randomizedpushgossip.json &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=20  --model=uniform --metrics-json-path=nodes-8-model-uniform-vertexNumber-10000-percentage-20-algorithm-randomizedpushgossip.json &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=25 --model=uniform --metrics-json-path=nodes-16-model-uniform-vertexNumber-10000-percentage-25-algorithm-randomizedpushgossip.json

#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=5 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=5 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=5 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=5 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16 -shared-heap 1G ${PGAS_BINARY_PATH} --vertex-count=10000 --percentage=5 &&
#exit 
#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=5 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=5 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=5 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=5 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=5 &&
#
#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=10 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=10 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=10 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=10 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=10 &&
#
#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=15 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=15 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=15 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=15 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=15 &&
#
#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=20 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=20 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=20 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=20 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=20 &&
#
#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=25 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=25 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=25 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=25 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=25 &&
#
#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=30 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=30 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=30 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=30 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=30 &&
#
#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=35 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=35 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=35 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=35 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=35 &&
#
#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=40 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=40 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=40 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=40 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=40 &&
#
#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=45 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=45 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=45 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=45 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=45 &&
#
#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=50 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=50 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=50 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=50 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=50 &&
#
#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=55 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=55 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=55 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=55 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=55 &&
#
#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=60 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=60 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=60 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=60 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=60 &&
#
#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=65 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=65 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=65 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=65 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=65 &&
#
#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=70 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=70 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=70 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=70 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=70 &&
#
#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=75 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=75 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=75 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=75 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=75 &&
#
#$UPCXX_INSTALL/bin/upcxx-run -n 1  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=80 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 2  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=80 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 4  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=80 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 8  -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=80 &&
#$UPCXX_INSTALL/bin/upcxx-run -n 16 -shared-heap 1G ./../build/pgas-graph-new-storage --vertex-count=10000 --percentage=80
#