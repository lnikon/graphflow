## Getting Started

# Requirements
Make sure that you've installed upcxx and have correctly set $UPCXX_INSTALL environment variable, then:
For more information see official upcxx wiki and INSTALL.md [upcxx](https://bitbucket.org/berkeleylab/upcxx/wiki/Home).
# Build


```sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -G"Ninja" -DCMAKE_CXX_COMPILER=/usr/bin/g++ 
ninja
```

After these steps executable pgas-graph-test will be generated, execute it using upcxx-run wrapper e.g.

```sh
upcxx-run -n4 ./pgas-graph-test
```
