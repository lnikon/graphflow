cmake_minimum_required (VERSION 3.8)
project (pgasgraph)

# Support for custom CMake modules.
list(
    APPEND 
    CMAKE_PREFIX_PATH 
    "${CMAKE_SOURCE_DIR}/lib/cmake"
)

#### Start PAPI ####
# Declare PAPI Content.
set(
    PAPI_DOWNLOAD_DIR "${CMAKE_SOURCE_DIR}/thirdparty/papi"
)

set(
    PAPI_SOURCE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/papi/src"
)

set(
    PAPI_LIB_DIR "${CMAKE_SOURCE_DIR}/thirdparty/papi/src/install/lib"
)

message("** PAPI download dir is: ${PAPI_DOWNLOAD_DIR}")
message("** PAPI source dir is: ${PAPI_SOURCE_DIR}")
message("** PAPI lib dir is: ${PAPI_LIB_DIR}")

include(FetchContent)
FetchContent_Declare(
    PAPI
    GIT_REPOSITORY https://bitbucket.org/icl/papi.git
    SOURCE_DIR ${PAPI_DOWNLOAD_DIR}
)

# Download the PAPI.
FetchContent_GetProperties(PAPI)
if(NOT PAPI_POPULATED)
    FetchContent_MakeAvailable(PAPI)
endif()

# Build the papi using a external script.
set(
    PAPI_BUILD_SCRIPT_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/BuildPAPI.sh"
)
execute_process(
    COMMAND 
    ${PAPI_BUILD_SCRIPT_PATH} 
    WORKING_DIRECTORY 
    ${CMAKE_CURRENT_SOURCE_DIR}
) 

# Add PAPI libs to the linker search paths.
link_directories(
    ${PAPI_LIB_DIR}
)

# Try to compile and link a sample PAPI program.
set(
    PAPI_TEST_SRC 
    "${CMAKE_SOURCE_DIR}/tests/PAPITest.cpp"
)
add_executable(
    PAPITest 
    ${PAPI_TEST_SRC}
)
target_include_directories(
    PAPITest 
    PUBLIC 
    ${PAPI_SOURCE_DIR}
)
target_link_libraries(
    PAPITest 
    libpapi
)
#### End PAPI ####

#### Start UPCXX ####
if(DEFINED ENV{UPCXX_INSTALL})
  list(APPEND CMAKE_PREFIX_PATH $ENV{UPCXX_INSTALL} )
endif()

set(CMAKE_CXX_COMPILER "/usr/bin/g++")
find_package(UPCXX 2020.3.2 REQUIRED)
message("FOUND UPCXX_VERSION=${UPCXX_VERSION}" )
#### End UPCXX ####

#### Stat Conan ####
if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conaninfo.txt)
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
endif()
find_package(Boost 1.74 REQUIRED COMPONENTS graph system unit_test_framework)
#### End Conan ####

#### Start PGASGraph ####
# Export PGASGraph as a static library
set(
    LIB_SOURCES
    "${CMAKE_SOURCE_DIR}/src/pgas-graph.cpp"
)
add_library(
    pgas-graph 
    ${LIB_SOURCES}
)
target_include_directories(
    pgas-graph
    PUBLIC
    inc
)
target_link_libraries(
    pgas-graph 
    PRIVATE 
    UPCXX::upcxx
    Boost::Boost
)

# Export GraphUtilities as a static library
set(
    GRAPH_UTILITIES_LIB_SOURCES
    "${CMAKE_SOURCE_DIR}/src/graph-utilities.cpp"
)
add_library(
    graph-utilities 
    ${GRAPH_UTILITIES_LIB_SOURCES}
)
target_include_directories(
    graph-utilities
    PUBLIC
    inc
)
target_link_libraries(
    graph-utilities 
    PRIVATE 
    UPCXX::upcxx
    Boost::Boost
)

# Driver program to test PGASGraph
# TODO Move into set of unit tests and benchmarks
set(
    SOURCES
    "${CMAKE_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_SOURCE_DIR}/src/graph-utilities.cpp"
)
add_executable(
    pgas-graph-test 
    ${SOURCES}
)
target_include_directories(
    pgas-graph-test
    PUBLIC
    inc
)
target_link_libraries(
    pgas-graph-test 
    PUBLIC 
    UPCXX::upcxx 
    Boost::Boost
    pgas-graph
    graph-utilities
)

# Program to generate graphs
# TODO Move into set of unit tests and benchmarks
set(
    GRAPH_GENERATOR_SOURCES
    "${CMAKE_SOURCE_DIR}/src/generate-graph.cpp"
)
add_executable(
    pgas-graph-generate
    ${GRAPH_GENERATOR_SOURCES}
)
target_include_directories(
    pgas-graph-generate
    PUBLIC
    inc
)
target_link_libraries(
    pgas-graph-generate 
    PUBLIC 
    UPCXX::upcxx 
    Boost::Boost
    pgas-graph
    graph-utilities
)

# Program to test new graph storage.
set(
	GRAPH_NEW_STORAGE_SOURCES
    "${CMAKE_SOURCE_DIR}/src/new-storage-graph.cpp"
)
add_executable(
    pgas-graph-new-storage
    ${GRAPH_NEW_STORAGE_SOURCES}
)
target_include_directories(
    pgas-graph-new-storage
    PUBLIC
    inc
)
target_link_libraries(
    pgas-graph-new-storage 
    PUBLIC 
    UPCXX::upcxx 
    Boost::Boost
    pgas-graph
    graph-utilities
)

# Program to make experimentations with the BGL
# TODO Move into set of unit tests and benchmarks
set(
    BGL_EXPERIMENTS_SOURCES
    "${CMAKE_SOURCE_DIR}/src/bgl-experiments.cpp"
)
add_executable(
    bgl-experiments
    ${BGL_EXPERIMENTS_SOURCES}
)
target_include_directories(
    bgl-experiments
    PUBLIC
    inc
)
target_link_libraries(
    bgl-experiments 
    PUBLIC 
    UPCXX::upcxx 
    Boost::Boost
    pgas-graph
    graph-utilities
)

# Program to make experimentations with the BGL MST
# TODO Move into set of unit tests and benchmarks
set(
    BGL_MST_EXPERIMENTS_SOURCES
    "${CMAKE_SOURCE_DIR}/src/boost_main.cpp"
)
add_executable(
    bgl-mst-experiments
    ${BGL_MST_EXPERIMENTS_SOURCES}
)
target_include_directories(
    bgl-mst-experiments
    PUBLIC
    inc
)
target_link_libraries(
    bgl-mst-experiments 
    PUBLIC 
    UPCXX::upcxx 
    Boost::Boost
    pgas-graph
    graph-utilities
)
#### End PGASGraph ####
