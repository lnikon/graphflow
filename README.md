## Description

PGASGraph is a library that provides distributed graph algorithms and data structures. This library uses distributed vertex store as a underlying graph storage.
PGASGraph is based on a [UPCXX](https://github.com/brycelelbach/upcxx) and uses Remote Procedure Calls (RPC) for communication, distributed objects, and global pointers provided by the UPCXX.

## Supported graph storages

+ \+ Distributed Vertex Store

## Supported distributed algorithms

+ \+ MST: Prim
- \- MST: Kruskal
- \- MST: GHS
- \- Shortest paths
- \- Clustering
- \- Centrality
- \- Link analysis
- \- ... more to come :)


## Requirements

Make sure that you've installed UPCXX and have correctly set `$UPCXX_INSTALL` environment variable.
For more information see official UPCXX wiki and INSTALL.md [UPCXX](https://bitbucket.org/berkeleylab/upcxx/wiki/Home).

## Build

### Dependencies

Library depends on Boost for program options parsing and CPU timer. Make sure that you've Boost libraries installed.

### Cmake/ninja/make

Library can be built using [Cmake](https://cmake.org/). Following commands can be used to build the library using Cmake and [ninja](https://ninja-build.org/).
Though, the use of ninja is not necessary and it can be safely replaced with usual [make](https://www.gnu.org/software/make/manual/make.html).

```sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -G"Ninja" -DCMAKE_CXX_COMPILER=/usr/bin/g++ 
ninja
```

After these steps executable pgas-graph-test will be generated, execute it using upcxx-run wrapper e.g.

```sh
upcxx-run -n4 ./pgas-graph-test
```

### Manual
Using the following instructions you can build library manually. *Once again, make sure that you've correctly set the `$UPCXX_INSTALL` variable.*

```sh
mkdir build && cd build
$UPCXX_INSTALL/bin/upcxx -codemode=opt -threadmode=seq -network=udp -std=c++17 -O -I../inc ../src/main.cpp ../src/graph-utilities.cpp ../src/pgas-graph.cpp -lboost_program_options -lboost_timer -o custom_build
```

Important UPCXX options here are `codemode`, `threadmode` and `network`.
As the library uses modern C++ features such as structured bindings or `auto` lambdas make sure to compile it with the `-std=c++17` flag.

## Distributed run notes

If you wish to make *distributed run* then make sure that you've compiled UPCXX with UDP or InfiniBand conduits.
Then make sure that you've correctly set the following variables. `$SSH_SERVERS` and `$GASNET_SSH_SERVERS` 
contain IP's of those nodes which will participate in a distributed computation. 
`$GASNET_MASTERIP` contains IP of the node which initiates the computation. **It's very important to set this variable.**

`$SSH_SERVERS=111.111.11.11,111.111.11.12`

`$GASNET_SSH_SERVERS=${SSH_SERVERS}`

`$GASNET_MASTERIP=10.0.0.30`
