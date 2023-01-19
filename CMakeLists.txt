cmake_minimum_required(VERSION 3.23)
project(
	PGASGraph
	VERSION 1.0
	LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

message("[PGASGraph] Using UPCXX_INSTALL=$ENV{UPCXX_INSTALL}")
if(DEFINED ENV{UPCXX_INSTALL})
  list(APPEND CMAKE_PREFIX_PATH $ENV{UPCXX_INSTALL}/share/cmake/UPCXX)
endif()

find_package(UPCXX REQUIRED)

if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conaninfo.txt)
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
endif ()

find_package(Boost COMPONENTS program_options REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(fmt REQUIRED)

set(pgasgraph_sources "${CMAKE_SOURCE_DIR}/include/pgasgraph/*.hpp")

# Treat each standalone as an separate project
add_subdirectory(${CMAKE_SOURCE_DIR}/standalone)
add_subdirectory(${CMAKE_SOURCE_DIR}/include)
