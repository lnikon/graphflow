cmake_minimum_required (VERSION 3.6)
project (pgasgraph)

if(DEFINED ENV{UPCXX_INSTALL})
  list(APPEND CMAKE_PREFIX_PATH $ENV{UPCXX_INSTALL} )
endif()

# UPCXX
find_package(UPCXX 2020.3.2 REQUIRED)
message("FOUND UPCXX_VERSION=${UPCXX_VERSION}" )

# Dependencies
if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conaninfo.txt)
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
endif()
find_package(Boost 1.74 REQUIRED COMPONENTS graph system unit_test_framework)

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
