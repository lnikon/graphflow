cmake_minimum_required(VERSION 3.8)
project(pgasgraph)

# Export PGASGraph as a static library
# add_library(pgas-graph ${LIB_SOURCES})
# target_include_directories(pgas-graph PUBLIC "${CMAKE_SOURCE_DIR}/inc" "${PAPI_SOURCE_DIR}")
# target_link_libraries(pgas-graph PRIVATE UPCXX::upcxx Boost::Boost) # performance-monitor libpapi)

find_package(Boost REQUIRED)
find_package(Poco REQUIRED)

set(COMPUTATION_SERVER_SOURCES "ComputationServer.cpp")
add_library(ComputationServer ${COMPUTATION_SERVER_SOURCES})
target_include_directories(ComputationServer PUBLIC "${CMAKE_SOURCE_DIR}/inc")
target_link_libraries(ComputationServer PUBLIC Boost::Boost Poco::Net)
