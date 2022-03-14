cmake_minimum_required(VERSION 3.8)
project(pgasgraph)

# Export PGASGraph as a static library
set(LIB_SOURCES "pgas-graph.cpp")

add_library(pgas-graph ${LIB_SOURCES})
target_include_directories(pgas-graph PUBLIC "${CMAKE_SOURCE_DIR}/inc" "${PAPI_SOURCE_DIR}")
target_link_libraries(pgas-graph PRIVATE UPCXX::upcxx Boost::Boost) # performance-monitor libpapi)

set(GRAPH_NEW_STORAGE_SOURCES "new-storage-graph.cpp")
add_executable(pgas-graph-new-storage ${GRAPH_NEW_STORAGE_SOURCES})
target_include_directories(pgas-graph-new-storage PUBLIC "${CMAKE_SOURCE_DIR}/inc")
target_link_libraries(pgas-graph-new-storage PUBLIC UPCXX::upcxx pgas-graph Boost::Boost)
