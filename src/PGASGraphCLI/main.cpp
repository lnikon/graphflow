// PGASGraph
#include <generators/knodel/knodel.h>
#include <generators/uniform/uniform.h>
#include <algorithms/gossip/gossip.h>
#include <pgas-graph/pgas-graph.h>

// CopyPasted
// #include <pgas-graph/cppmemusage.hpp>

// STL
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <functional>

// Boost

// System
#include <limits.h>
#include <unistd.h>

// Usings

bool Test_ConstructGraph1()
{
    using VertexData = std::string;
    using EdgeData = std::string;

    const std::size_t vertexCount{ 16 };
    const std::size_t vertexCountPerRank{ vertexCount / upcxx::rank_n() };

    PGASGraph::Graph<PGASGraph::Generators::Knodel::Vertex<VertexData, EdgeData>,
        PGASGraph::Generators::Knodel::Edge<EdgeData>>
        graph(vertexCount, vertexCountPerRank);

    return true;
}

bool Test_AddVertex1()
{
    using VertexData = int;
    using EdgeData = int;

    const std::size_t vertexCount{ 16 };
    const std::size_t vertexCountPerRank{ vertexCount / upcxx::rank_n() };

    PGASGraph::Graph<PGASGraph::Generators::Knodel::Vertex<VertexData, EdgeData>,
        PGASGraph::Generators::Knodel::Edge<EdgeData>>
        graph(vertexCount, vertexCountPerRank);

    PGASGraph::Generators::Knodel::VertexId from{ 0, 1, vertexCount / 2 };
    PGASGraph::Generators::Knodel::VertexId to{ 1, 2, vertexCount / 2 };

    graph.AddEdge({ from, to, 0 });
    return true;
}

bool Test_AddVertex2()
{
    using VertexData = int;
    using EdgeData = int;

    const std::size_t vertexCount{ 16 };
    const std::size_t vertexCountPerRank{ vertexCount / upcxx::rank_n() };

    PGASGraph::Graph<PGASGraph::Generators::Knodel::Vertex<VertexData, EdgeData>,
        PGASGraph::Generators::Knodel::Edge<EdgeData>>
        graph(vertexCount, vertexCountPerRank);

    PGASGraph::Generators::Knodel::VertexId from{ 0, 1, vertexCount / 2 };
    PGASGraph::Generators::Knodel::VertexId to{ 1, 2, vertexCount / 2 };

    graph.AddEdge({ from, to, 0 });
    if (graph.HasEdge(from, to))
    {
        std::cout << "Edge exists between from=" << from.ToString() << " and to=" << to.ToString()
            << std::endl;
    }

    return true;
}

bool Test_ExportGraph1()
{
    using VertexData = int;
    using EdgeData = int;

    const std::size_t vertexCount{ 16 };
    const std::size_t vertexCountPerRank{ vertexCount / upcxx::rank_n() };

    std::string filename("exported-graph.txt");

    PGASGraph::Graph<PGASGraph::Generators::Knodel::Vertex<VertexData, EdgeData>,
        PGASGraph::Generators::Knodel::Edge<EdgeData>>
        graph(vertexCount, vertexCountPerRank);

    PGASGraph::Generators::Knodel::VertexId from{ 0, 1, vertexCount / 2 };
    PGASGraph::Generators::Knodel::VertexId to{ 1, 2, vertexCount / 2 };

    graph.AddEdge({ from, to, 0 });
    graph.ExportIntoFile(filename);

    return true;
}

bool Test_GenerateKnodel1() {
    using VertexData = int;
    using EdgeData = int;

    const std::size_t vertexCount{ 8 };
    const std::size_t vertexCountPerRank{ vertexCount / upcxx::rank_n() };
    const std::size_t delta{ static_cast<std::size_t>(std::log2(vertexCount)) };

    std::string filename("exported-knodel-graph.txt");

    PGASGraph::Graph<PGASGraph::Generators::Knodel::Vertex<VertexData, EdgeData>,
        PGASGraph::Generators::Knodel::Edge<EdgeData>>
        graph(64, 64);

    PGASGraph::Generators::Knodel::Generate(vertexCount, vertexCount, 2, graph);
    graph.ExportIntoFile(filename);

    return true;
}

bool Test_RandomizedPushPullGossip1() {
    using VertexData = int;
    using EdgeData = int;

    const std::size_t vertexCount{ 1024 };
    const size_t vertexCountPerRank{ (vertexCount + upcxx::rank_n() - 1) / upcxx::rank_n() };
    const std::size_t delta{ static_cast<std::size_t>(std::log2(vertexCount)) };
    const std::size_t deltaPerRank{ delta / upcxx::rank_n() };

    std::string filename("exported-knodel-graph.txt");

    PGASGraph::Graph<PGASGraph::Generators::Knodel::Vertex<VertexData, EdgeData>,
        PGASGraph::Generators::Knodel::Edge<EdgeData>>
        graph(vertexCount, vertexCountPerRank);

    PGASGraph::Generators::Knodel::Generate(vertexCount, vertexCountPerRank, delta, graph);
    upcxx::barrier();
    PGASGraph::Generators::Knodel::VertexId vertexId1{ 0, 0, vertexCount / 2 };
    upcxx::barrier();
    PGASGraph::Algorithms::Gossip::PushRandomizedGossip(graph, vertexId1, 15);
    upcxx::barrier();
    graph.ExportIntoFile(filename);
    upcxx::barrier();
    return true;
}

// TODO: Actually, there are highly noticable connected component, this is not that much random...
bool Test_GenerateUniformRandomGraph1() {
    using VertexData = long unsigned int;
    using EdgeData = long unsigned int;

    const std::size_t vertexCount{ 32 };
    const std::size_t vertexCountPerRank{ vertexCount / upcxx::rank_n() };
    const std::size_t delta{ static_cast<std::size_t>(std::log2(vertexCount)) };

    std::string filename("exported-uniform-graph.txt");

    PGASGraph::Graph<PGASGraph::Generators::Uniform::Vertex<VertexData, EdgeData>,
        PGASGraph::Generators::Uniform::Edge<EdgeData>>
        graph(64, 64);

    PGASGraph::Generators::Uniform::Generate(16, 80.0, graph);
    // PGASGraph::Generators::Uniform::VertexId vertexId1{0, 0};
    // PGASGraph::Algorithms::Gossip::PushRandomizedGossip(graph, vertexId1, 15);
    graph.ExportIntoFile(filename);
    return true;
}

bool Test_RandomizedPushOnUniformRandomGraph1() {
    using VertexData = long unsigned int;
    using EdgeData = long unsigned int;

    const std::size_t vertexCount{ 16 };
    const size_t vertexCountPerRank{ (vertexCount + upcxx::rank_n() - 1) / upcxx::rank_n() };
    const std::size_t delta{ static_cast<std::size_t>(std::log2(vertexCount)) };

    const std::string verticesFilename("exported-uniform-vertices-after-push.txt");
    const std::string edgesFilename("exported-uniform-edges-after-push.txt");

    PGASGraph::Graph<PGASGraph::Generators::Uniform::Vertex<VertexData, EdgeData>,
        PGASGraph::Generators::Uniform::Edge<EdgeData>>
        graph(vertexCount, vertexCountPerRank);

    upcxx::barrier();
    PGASGraph::Generators::Uniform::Generate(vertexCountPerRank, 100.0, graph);
    upcxx::barrier();
    PGASGraph::Generators::Uniform::VertexId vertexId1(0);
    upcxx::barrier();
    PGASGraph::Algorithms::Gossip::PushRandomizedGossip(graph, vertexId1, 15);
    upcxx::barrier();
    graph.ExportVerticesIntoFile(verticesFilename);
    upcxx::barrier();
    graph.ExportIntoFile(edgesFilename);
    upcxx::barrier();
    return true;
}

bool Test_RandomizedPushOnKnodelGraph1() {
    using VertexData = int;
    using EdgeData = int;

    const std::size_t vertexCount{ 8 };
    const size_t vertexCountPerRank{ (vertexCount + upcxx::rank_n() - 1) / upcxx::rank_n() };
    const std::size_t delta{ static_cast<std::size_t>(std::log2(vertexCount)) };
    const std::size_t deltaPerRank{ static_cast<std::size_t>(std::log2(vertexCountPerRank)) };

    const std::string verticesFilename("exported-knodel-vertices-after-push.txt");
    const std::string edgesFilename("exported-knodel-edges-after-push.txt");

    PGASGraph::Graph<PGASGraph::Generators::Knodel::Vertex<VertexData, EdgeData>,
        PGASGraph::Generators::Knodel::Edge<EdgeData>>
        graph(vertexCount, vertexCountPerRank);

    PGASGraph::Generators::Knodel::Generate(vertexCount, vertexCount, delta, graph);

    upcxx::barrier();
    PGASGraph::Generators::Knodel::VertexId vertexId1(0, 0, vertexCount / 2);
    upcxx::barrier();
    PGASGraph::Algorithms::Gossip::PushRandomizedGossip(graph, vertexId1, 15);
    //graph.ExportVerticesIntoFile(verticesFilename);
    //graph.ExportIntoFile(edgesFilename);
    return true;
}

bool Test_BroadcastOnUniformRandomGraph1() {
    using VertexData = long unsigned int;
    using EdgeData = long unsigned int;

    const std::size_t vertexCount{ 16 };
    const size_t vertexCountPerRank{ (vertexCount + upcxx::rank_n() - 1) / upcxx::rank_n() };
    const std::size_t delta{ static_cast<std::size_t>(std::log2(vertexCount)) };

    const std::string verticesFilename("exported-uniform-vertices-after-push.txt");
    const std::string edgesFilename("exported-uniform-edges-after-push.txt");

    PGASGraph::Graph<PGASGraph::Generators::Uniform::Vertex<VertexData, EdgeData>,
        PGASGraph::Generators::Uniform::Edge<EdgeData>>
        graph(vertexCount, vertexCountPerRank);

    upcxx::barrier();
    PGASGraph::Generators::Uniform::Generate(vertexCountPerRank, 100.0, graph);
    upcxx::barrier();
    PGASGraph::Generators::Uniform::VertexId vertexId1(0);
    upcxx::barrier();
    PGASGraph::Algorithms::Gossip::BroadcastGossip(graph, vertexId1, 15);
    upcxx::barrier();
    graph.ExportVerticesIntoFile(verticesFilename);
    upcxx::barrier();
    graph.ExportIntoFile(edgesFilename);
    upcxx::barrier();
    return true;
}

bool Test_BroadcastOnKnodelGraph1() {
    using VertexData = int;
    using EdgeData = int;

    const std::size_t vertexCount{ 1024 };
    const size_t vertexCountPerRank{ (vertexCount + upcxx::rank_n() - 1) / upcxx::rank_n() };
    const std::size_t delta{ static_cast<std::size_t>(std::log2(vertexCount)) };
    const std::size_t deltaPerRank{ delta / upcxx::rank_n() };

    const std::string verticesFilename("/home/nikon/projects/phd/pgas-graph/build/exported-knodel-vertices-after-push.txt");
    const std::string edgesFilename("/home/nikon/projects/phd/pgas-graph/build/exported-knodel-edges-after-push.txt");

    PGASGraph::Graph<PGASGraph::Generators::Knodel::Vertex<VertexData, EdgeData>,
        PGASGraph::Generators::Knodel::Edge<EdgeData>>
        graph(vertexCount, vertexCountPerRank);

    //upcxx::barrier();
    // TODO: Which delta to supply?
    PGASGraph::Generators::Knodel::Generate(vertexCount, vertexCountPerRank, delta, graph);

    // upcxx::barrier();
    PGASGraph::Generators::Knodel::VertexId vertexId1(0, 0, vertexCount / 2);
    upcxx::barrier();
    //if (upcxx::rank_me() == 0) {
    //    graph.printLocal();
    //}
    //upcxx::barrier();
    //if (upcxx::rank_me() == 1) {
    //    graph.printLocal();
    //}
    PGASGraph::Algorithms::Gossip::PushRandomizedGossip(graph, vertexId1, 15);
    upcxx::barrier();
    //graph.ExportVerticesIntoFile(verticesFilename);
    graph.ExportIntoFile(edgesFilename);
    return true;
}

struct TestCase {
    std::function<bool()> test;
    std::string name;
};

int main(int argc, char* argv[])
{
    upcxx::init();

    std::vector<TestCase> tests;

    //tests.push_back({ Test_ConstructGraph1, "Test_ConstructGraph1" });
    //tests.push_back({ Test_AddVertex1, "Test_AddVertex1" });
    //tests.push_back({ Test_AddVertex2, "Test_AddVertex2" });
    //tests.push_back({ Test_ExportGraph1, "Test_ExportGraph1" });
    //tests.push_back({ Test_GenerateKnodel1, "Test_GenerateKnodel1" });
    //Test_RandomizedPushPullGossip1();
    //tests.push_back({ Test_RandomizedPushPullGossip1, "Test_RandomizedPushPullGossip1" });
    //tests.push_back({ Test_GenerateUniformRandomGraph1, "Test_GenerateUniformRandomGraph1" });
    //tests.push_back({ Test_RandomizedPushOnUniformRandomGraph1, "Test_RandomizedPushOnUniformRandomGraph1" });
    //tests.push_back({ Test_RandomizedPushOnKnodelGraph1, "Test_RandomizedPushOnKnodelGraph1" });
    //tests.push_back({ Test_BroadcastOnUniformRandomGraph1, "Test_BroadcastOnUniformRandomGraph1" });
    //tests.push_back({ Test_BroadcastOnKnodelGraph1, "Test_BroadcastOnKnodelGraph1" });
    Test_BroadcastOnKnodelGraph1();

    for (auto& test : tests) {
        if (upcxx::rank_me() == 0) {
            std::cout << "Starting " << test.name << std::endl;
        }

        test.test();

        if (upcxx::rank_me() == 0) {
            std::cout << "Finishing " << test.name << std::endl << std::endl;
        }
    }

    upcxx::finalize();
    return 0;
};
