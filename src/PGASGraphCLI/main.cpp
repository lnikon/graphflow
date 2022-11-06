// PGASGraph
#include <generators/knodel/knodel.h>
#include <pgas-graph/pgas-graph.h>

// CopyPasted
// #include <pgas-graph/cppmemusage.hpp>

// STL
#include <chrono>
#include <iostream>
#include <random>

// Boost

// System
#include <limits.h>
#include <unistd.h>

// Usings

bool Test_ConstructGraph1()
{
    using VertexData = std::string;
    using EdgeData = std::string;

    const std::size_t vertexCount{16};
    const std::size_t vertexCountPerRank{vertexCount / upcxx::rank_n()};

    PGASGraph::Graph<PGASGraph::Generators::Knodel::Vertex<VertexData, EdgeData>,
                     PGASGraph::Generators::Knodel::Edge<EdgeData>>
        graph(vertexCount, vertexCountPerRank);

    return true;
}

bool Test_AddVertex1()
{
    using VertexData = int;
    using EdgeData = int;

    const std::size_t vertexCount{16};
    const std::size_t vertexCountPerRank{vertexCount / upcxx::rank_n()};

    PGASGraph::Graph<PGASGraph::Generators::Knodel::Vertex<VertexData, EdgeData>,
                     PGASGraph::Generators::Knodel::Edge<EdgeData>>
        graph(vertexCount, vertexCountPerRank);

    PGASGraph::Generators::Knodel::VertexId from{0, 1};
    PGASGraph::Generators::Knodel::VertexId to{1, 2};

    graph.AddEdge({from, to, 0});
    return true;
}

bool Test_AddVertex2()
{
    using VertexData = int;
    using EdgeData = int;

    const std::size_t vertexCount{16};
    const std::size_t vertexCountPerRank{vertexCount / upcxx::rank_n()};

    PGASGraph::Graph<PGASGraph::Generators::Knodel::Vertex<VertexData, EdgeData>,
                     PGASGraph::Generators::Knodel::Edge<EdgeData>>
        graph(vertexCount, vertexCountPerRank);

    PGASGraph::Generators::Knodel::VertexId from{0, 1};
    PGASGraph::Generators::Knodel::VertexId to{1, 2};

    graph.AddEdge({from, to, 0});
    if (graph.HasEdge(from, to))
    {
        std::cout << "Edge exists between from=" << from.ToString() << " and to=" << to.ToString()
                  << std::endl;
    }

    return true;
}

int main(int argc, char* argv[])
{
    upcxx::init();

    // Test_ConstructGraph1();
    // Test_AddVertex1();
    Test_AddVertex2();

    upcxx::finalize();
    return 0;
};
