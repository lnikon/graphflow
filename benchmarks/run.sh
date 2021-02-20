#!/bin/bash

testCount=9
vertexCount=(64 256 1024 4096 8192 16384 33668 67336 134672)
percentages=(100 100 100 25 10 0.01 0.01 0.01 0.001)
rankCount=(1 2)

# Run simulations.
##for idx in ${!vertexCount[@]};
##do
##	for rank in ${rankCount[@]};
##	do
##		fileName="v${vertexCount[$idx]}_n${rank}_p${percentages[$idx]}.topout"
##
##		# Launch simulation.
##		$UPCXX_INSTALL/bin/upcxx-run -n ${rank} ../build/pgas-graph-test --vertex-count=${vertexCount[$idx]} --percentage=${percentages[$idx]} &
##
##		# Monitor process CPU and MEM usage.
##		echo "Current log file: ${fileName}"
##		while kill -0 $(pgrep pgas-graph-test) 2> /dev/null;
##		do
##			ps -C pgas-graph-test -o %cpu,%mem,cmd >> ${fileName}
##		done  
##		sleep 5
##	done
##done
##
### Parse CPU/MEM usage data.
##for idx in ${!vertexCount[@]};
##do
##	for rank in ${rankCount[@]};
##	do
##		fileName="v${vertexCount[$idx]}_n${rank}_p${percentages[$idx]}.topout"
##		maxMem=$(sed '/^%/ d' ${fileName} | sort -k 1 | tail -n 1 | awk '{ print $1 }')
##		maxCpu=$(sed '/^%/ d' ${fileName} | sort -k 2 | tail -n 1 | awk '{ print $2 }')
##		echo "V=${vertexCount[$idx]}, R=${rank}, P=${percentages[$idx]}, MAX_MEM=${maxMem}, MAX_CPU=${maxCpu}"
##	done
##done


$UPCXX_INSTALL/bin/upcxx-run -n 1 ../build/pgas-graph-test --vertex-count=64 --percentage=100 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../build/pgas-graph-test --vertex-count=64 --percentage=100 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../build/pgas-graph-test --vertex-count=256 --percentage=100 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../build/pgas-graph-test --vertex-count=256 --percentage=100 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../build/pgas-graph-test --vertex-count=1024 --percentage=100 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../build/pgas-graph-test --vertex-count=1024 --percentage=100 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../build/pgas-graph-test --vertex-count=2048 --percentage=50 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../build/pgas-graph-test --vertex-count=2048 --percentage=50 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../build/pgas-graph-test --vertex-count=4096 --percentage=25 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../build/pgas-graph-test --vertex-count=4096 --percentage=25 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../build/pgas-graph-test --vertex-count=8192 --percentage=10 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../build/pgas-graph-test --vertex-count=8192 --percentage=10 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../build/pgas-graph-test --vertex-count=16384 --percentage=1 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../build/pgas-graph-test --vertex-count=16384 --percentage=1 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../build/pgas-graph-test --vertex-count=33668 --percentage=0.1 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../build/pgas-graph-test --vertex-count=33668 --percentage=0.1 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../build/pgas-graph-test --vertex-count=67336 --percentage=0.01 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../build/pgas-graph-test --vertex-count=67336 --percentage=0.01 &&

$UPCXX_INSTALL/bin/upcxx-run -n 1 ../build/pgas-graph-test --vertex-count=134672 --percentage=0.001 &&
$UPCXX_INSTALL/bin/upcxx-run -n 2 ../build/pgas-graph-test --vertex-count=134672 --percentage=0.001

