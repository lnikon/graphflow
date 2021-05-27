cmake_minimum_required(VERSION 3.8)
project(pgasgraph)

# Execute Conan.
set(
        CONAN_SCRIPT_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/ExecuteConan.sh"
)
execute_process(
        COMMAND
        ${CONAN_SCRIPT_PATH}
        WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR})

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

# include(FetchContent)
# FetchContent_Declare(
#         PAPI
#         GIT_REPOSITORY https://bitbucket.org/icl/papi.git
#         SOURCE_DIR ${PAPI_DOWNLOAD_DIR}
# )

# # Download the PAPI.
# FetchContent_GetProperties(PAPI)
# if (NOT PAPI_POPULATED)
#     FetchContent_MakeAvailable(PAPI)
# endif ()

# Build the papi using a external script.
set(
        PAPI_BUILD_SCRIPT_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/BuildPAPI.sh"
)
# execute_process(
#         COMMAND
#         ${PAPI_BUILD_SCRIPT_PATH}
#         WORKING_DIRECTORY
#         ${CMAKE_CURRENT_SOURCE_DIR}
# )

# Add PAPI libs to the linker search paths.
link_directories(
        ${PAPI_LIB_DIR}
)

# Try to compile and link a sample PAPI program.
# set(
#         PAPI_TEST_SRC
#         "${CMAKE_SOURCE_DIR}/tests/PAPITest.cpp"
# )
# add_executable(
#         PAPITest
#         ${PAPI_TEST_SRC}
# )
# target_include_directories(
#         PAPITest
#         PUBLIC
#         ${PAPI_SOURCE_DIR}
# )
# target_link_libraries(
#         PAPITest
#         libpapi
# )
#### End PAPI ####

#### Start UPCXX ####
if (DEFINED ENV{UPCXX_INSTALL})
    list(APPEND CMAKE_PREFIX_PATH $ENV{UPCXX_INSTALL})
endif ()

set(
        CMAKE_CXX_COMPILER "/usr/bin/g++"
)
set(
        CMAKE_EXPORT_COMPILE_COMMANDS
        "/usr/bin/g++"
)
find_package(
        UPCXX
        2021.3.0
        REQUIRED
)
message("FOUND UPCXX_VERSION=${UPCXX_VERSION}")
#### End UPCXX ####

### Stat Conan ####
if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conaninfo.txt)
    set(
            CMAKE_MODULE_PATH
            ${CMAKE_CURRENT_BINARY_DIR}
    )
endif ()
find_package(
        Boost
        1.74
        REQUIRED
        COMPONENTS
        program_options
)
### End Conan ####

#### Start PGASGraph ####
# # Export PerformanceMonitor as a static library
# set(
#         PERFORMANCE_MONITOR_LIB_SOURCES
#         "${CMAKE_SOURCE_DIR}/src/performance_monitor.cpp"
# )
# add_library(
#         performance-monitor
#         ${PERFORMANCE_MONITOR_LIB_SOURCES}
# )
# target_include_directories(
#         performance-monitor
#         PUBLIC
#         inc
#         ${PAPI_SOURCE_DIR}
# )
# target_link_libraries(
#         performance-monitor
#         libpapi
# )

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
        ${PAPI_SOURCE_DIR}
)
target_link_libraries(
        pgas-graph
        PRIVATE
        UPCXX::upcxx
        Boost::Boost
        # performance-monitor
        # libpapi
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
        pgas-graph
        Boost::Boost
)
#### End PGASGraph ####
