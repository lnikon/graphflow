cmake_minimum_required(VERSION 3.23)
project(
	PGASGraph
	VERSION 1.0
	LANGUAGES C CXX
)

# General, CMake-related staff
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# UPCXX
if(DEFINED ENV{UPCXX_INSTALL})
  list(APPEND CMAKE_PREFIX_PATH $ENV{UPCXX_INSTALL}/share/cmake/UPCXX)
endif()

find_package(UPCXX REQUIRED)


# Conan
# Note: conan-io wrapper for Conan2 is still in experimental/development stage.
# conan_cmake_autodetect(settings)
# conan_cmake_install(
#   PATH_OR_REFERENCE ${CMAKE_SOURCE_DIR}
#   INSTALL_FOLDER ${CMAKE_BINARY_DIR}
#   BUILD missing
#   REMOTE conancenter
#   SETTINGS ${settings}
# 	ENV "CONAN_CMAKE_GENERATOR=${CMAKE_GENERATOR}"
# 	CONF "tools.cmake.cmaketoolchain:generator=${CMAKE_GENERATOR}"
# )

# if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conaninfo.txt)
#     set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
# endif ()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Boost COMPONENTS program_options REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)

set(pgasgraph_sources "${CMAKE_SOURCE_DIR}/include/pgasgraph/*.hpp")

# Treat each standalone as an separate project
add_subdirectory(${CMAKE_SOURCE_DIR}/standalone)
add_subdirectory(${CMAKE_SOURCE_DIR}/include)
