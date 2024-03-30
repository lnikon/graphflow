set(PGASGRAPH_CLI_SRC "main.cpp")

# TODO: Support build with multiple conduits
set(PGASGRAPH_CLI "pgas-graph-cli-${UPCXX_NETWORK}")
add_executable(${PGASGRAPH_CLI} ${PGASGRAPH_CLI_SRC})
target_include_directories(${PGASGRAPH_CLI} PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(${PGASGRAPH_CLI} PUBLIC UPCXX::upcxx Boost::Boost fmt::fmt PGASGraph)
