cmake_minimum_required(VERSION 3.8)
project(pgasgraph)

# Export PGASGraph as a static library
# set(LIB_SOURCES "pgas-graph.cpp")
# 
# add_library(pgas-graph ${LIB_SOURCES})
# target_include_directories(pgas-graph PUBLIC "${CMAKE_SOURCE_DIR}/inc" "${PAPI_SOURCE_DIR}")
# target_link_libraries(pgas-graph PRIVATE UPCXX::upcxx Boost::Boost) # performance-monitor libpapi)

set(PGASGRAPH_CLI_SRC "main.cpp"
                      "pgas-graph.cpp")
set(PGASGRAPH_CLI "pgas-graph-cli-${UPCXX_NETWORK}")

add_executable(${PGASGRAPH_CLI} ${PGASGRAPH_CLI_SRC})
target_include_directories(${PGASGRAPH_CLI} PUBLIC "${CMAKE_SOURCE_DIR}/inc")
target_link_libraries(${PGASGRAPH_CLI} PUBLIC UPCXX::upcxx Boost::Boost nlohmann_json::nlohmann_json fmt::fmt)

