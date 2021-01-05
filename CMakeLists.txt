# Simple test driver for UPCXX CMake package

cmake_minimum_required (VERSION 3.6)
project (pgasgraph)

if(DEFINED ENV{UPCXX_INSTALL})
  list(APPEND CMAKE_PREFIX_PATH $ENV{UPCXX_INSTALL} )
endif()

find_package(UPCXX 2020.3.2 REQUIRED)
message("FOUND UPCXX_VERSION=${UPCXX_VERSION}" )

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
)

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
	pgas-graph
)

message("SUCCESS")

