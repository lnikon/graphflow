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

# Driver program to test PGASGraph
# TODO Move into set of unit tests and benchmarks
set(
	TEST_SOURCES
	"${CMAKE_SOURCE_DIR}/src/main.cpp"
)
add_executable(
	pgas-graph-test 
	${TEST_SOURCES}
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
)

