## Build Instructions

Make sure that you've installed upcxx and have correctly set $UPCXX_INSTALL environment variable, then:

```sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -G"Ninja" -DCMAKE_CXX_COMPILER=/usr/bin/g++ 
ninja
```

After these steps executable pgas-graph-test will be generated, simply execute it. 
