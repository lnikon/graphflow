// PGASGraph
#include <pgasgraph/generators/knodel/knodel.h>
#include <pgasgraph/generators/uniform/uniform.h>
#include <pgasgraph/algorithms/gossip/gossip.h>
#include <pgasgraph/pgasgraph.h>

// CopyPasted
// #include <pgasgraph/cppmemusage.hpp>

// STL
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <functional>

// Boost
#include <boost/program_options.hpp>
#include <fmt/core.h>
#include <nlohmann/json.hpp>

// System
#include <limits.h>
#include <unistd.h>
#include <sys/wait.h>

// Usings
namespace po = boost::program_options;

std::string currentTime()
{
    std::time_t time = std::time({});
    char timeString[std::size("hh:mm:ss")];
    std::strftime(std::data(timeString), std::size(timeString), "%T", std::gmtime(&time));

    std::string s("asd");

    return timeString;
}

struct ProgramOptions
{
    size_t totalNumberVertices{ 256 };
    double percentage{ 5.0 };
    std::string algorithm;
    std::string model;
    bool printLocal{ false };
    std::string exportPath{};
    std::string metricsJsonPath{};
    bool usePapi{ false };
};

po::options_description CreateProgramOptions()
{
    po::options_description desc("Allowed options");
    desc.add_options()("help", "produce help message");
    desc.add_options()("vertex-count", po::value<int>(), "set vertex count");
    desc.add_options()("percentage", po::value<double>(), "set connectivity percentage");
    desc.add_options()("algorithm", po::value<std::string>(), "which algorithm to run");
    desc.add_options()("model", po::value<std::string>(), "which graph model to use");
    desc.add_options()("print-local", po::value<int>(), "print edges on the current node");
    desc.add_options()("export-path", po::value<std::string>(), "export graph as a edgelist into the file");
    desc.add_options()("metrics-json-path", po::value<std::string>(), "export simulation metrics e.g. generation time, edge count");
    desc.add_options()("use-papi", po::value<bool>(), "use PAPIs performance counters");

    return desc;
}

ProgramOptions ParseOptions(int argc, char** argv)
{
    // Parse command line options.
    auto optionsDesc = CreateProgramOptions();
    ProgramOptions result;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, optionsDesc), vm);
    if (vm.count("help"))
    {
        std::cout << optionsDesc << std::endl;
        return result;
    }

    // Get vertex count.
    if (vm.count("vertex-count"))
    {
        result.totalNumberVertices = vm["vertex-count"].as<int>();
    }

    // Get vertex count.
    if (vm.count("percentage"))
    {
        result.percentage = vm["percentage"].as<double>();
    }

    if (vm.count("algorithm"))
    {
        result.algorithm = vm["algorithm"].as<std::string>();
    }

    if (vm.count("model")) {
        result.model = vm["model"].as<std::string>();
    }

    // Get vertex count.
    if (vm.count("print-local"))
    {
        result.printLocal = vm["print-local"].as<int>();
    }

    // Graph export path.
    if (vm.count("export-path"))
    {
        result.exportPath = vm["export-path"].as<std::string>();
    }

    if (vm.count("metrics-json-path")) {
        result.metricsJsonPath = vm["metrics-json-path"].as<std::string>();
    }

    // Run PAPI or not.
    if (vm.count("use-papi"))
    {
        result.usePapi = vm["use-papi"].as<bool>();
    }

    return result;
}

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
    //PGASGraph::Algorithms::Gossip::PushRandomizedGossip(graph, vertexId1, 15);
    PGASGraph::Algorithms::Gossip::BroadcastGossip(graph, vertexId1, 15);
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

    const std::size_t vertexCount{ 64 };
    const size_t vertexCountPerRank{ (vertexCount + upcxx::rank_n() - 1) / upcxx::rank_n() };
    const std::size_t delta{ static_cast<std::size_t>(std::log2(vertexCount)) };
    const std::size_t deltaPerRank{ delta / upcxx::rank_n() };

    const std::string verticesFilename("/home/nikon/projects/phd/pgasgraph/build/exported-knodel-vertices-after-push.txt");
    const std::string edgesFilename("/home/nikon/projects/phd/pgasgraph/build/exported-knodel-edges-after-push.txt");

    PGASGraph::Graph<PGASGraph::Generators::Knodel::Vertex<VertexData, EdgeData>,
        PGASGraph::Generators::Knodel::Edge<EdgeData>>
        graph(vertexCount, vertexCountPerRank);

    //upcxx::barrier();
    // TODO: Which delta to supply?
    PGASGraph::Generators::Knodel::Generate(vertexCount, vertexCountPerRank, delta, graph);

    // upcxx::barrier();
    PGASGraph::Generators::Knodel::VertexId vertexId1(0, 0, vertexCount / 2);
    upcxx::barrier();
    graph.printLocal();
    //PGASGraph::Algorithms::Gossip::PushRandomizedGossip(graph, vertexId1, 15);
    PGASGraph::Algorithms::Gossip::BroadcastGossip(graph, vertexId1, 15);
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

    CreateProgramOptions();
    const auto options{ ParseOptions(argc, argv) };

    nlohmann::json resultJson;

    std::time_t timeNow = std::time(nullptr);
    char* dt = nullptr;
    if (0 == upcxx::rank_me())
    {
        // PGASGraph::logMsg("******************************************");
        // PGASGraph::logMsg("Start time: " + std::string(dt));
        resultJson["startTime"] = currentTime();
    }

    // Print hostname to make sure that the run is truly distributed.
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    // upcxx::rpc(0, [hostname]() { PGASGraph::logMsg(std::string{"Hostname: "} + hostname);
    // }).wait();

    upcxx::barrier();

    const auto programOptions{ ParseOptions(argc, argv) };
    if (upcxx::rank_me() == 0)
    {
        // PGASGraph::logMsg("Total number of vertices: " +
        //                   std::to_string(programOptions.totalNumberVertices));
        // PGASGraph::logMsg("Percentage: " + std::to_string(programOptions.percentage));
        resultJson["totalVertexCount"] = std::to_string(programOptions.totalNumberVertices);
        resultJson["percentage"] = std::to_string(programOptions.percentage);
    }

    // Distribute vertices over the ranks.
    const size_t verticesPerRank =
        (programOptions.totalNumberVertices + upcxx::rank_n() - 1) / upcxx::rank_n();
    resultJson["verticesPerRank"] = verticesPerRank;

    if (programOptions.model == "uniform") {
        using VertexData = long unsigned int;
        using EdgeData = long unsigned int;

        if (upcxx::rank_me() == 0) {
            resultJson["model"] = "uniform";
        }

        // Initialize the graph
        upcxx::barrier();
        PGASGraph::Graph<PGASGraph::Generators::Uniform::Vertex<VertexData, EdgeData>,
            PGASGraph::Generators::Uniform::Edge<EdgeData>>
            graph(programOptions.totalNumberVertices, verticesPerRank);

        upcxx::barrier();

        // Generate edges/vertices
        auto start = std::chrono::steady_clock::now();
        const auto edgeCount{ PGASGraph::Generators::Uniform::Generate(verticesPerRank, programOptions.percentage, graph) };
        auto end = std::chrono::steady_clock::now();
        auto took = std::chrono::duration<double>(end - start).count();

        // Collect results
        resultJson["counters"]["generationTime"][fmt::format("rank_{}", upcxx::rank_me())] = took;
        resultJson["counters"]["edgeCount"][fmt::format("rank_{}", upcxx::rank_me())] = edgeCount;

        upcxx::barrier();
        start = std::chrono::steady_clock::now();
        //if (programOptions.algorithm == "randomizedpushgossip")
        {
            PGASGraph::Generators::Uniform::VertexId vertexId(0);
            PGASGraph::Algorithms::Gossip::PushRandomizedGossip(graph, vertexId, 15);
        }
        end = std::chrono::steady_clock::now();
        took = std::chrono::duration<double>(end - start).count();
        resultJson["counters"]["simulationTime"]["randomizedPushGossip"][fmt::format("rank_{}", upcxx::rank_me())] = took;
        if (upcxx::rank_me() == 0) {
            resultJson["counters"]["simulationTime"]["randomizedPushGossip"]["algorithm"] = "RandomizedPushGossip";
        }

        upcxx::barrier();
        start = std::chrono::steady_clock::now();
        //if (programOptions.algorithm == "randomizedpushgossip")
        //else if (programOptions.algorithm == "broadcastgossip")
        {
            if (upcxx::rank_me() == 0) {
                resultJson["algorithm"] = "broadcastGossip";
            }

            PGASGraph::Generators::Uniform::VertexId vertexId(0);
            PGASGraph::Algorithms::Gossip::BroadcastGossip(graph, vertexId, 15);
        }
        end = std::chrono::steady_clock::now();
        took = std::chrono::duration<double>(end - start).count();
        resultJson["counters"]["simulationTime"]["broadcastGossip"][fmt::format("rank_{}", upcxx::rank_me())] = took;
        if (upcxx::rank_me() == 0) {
            resultJson["counters"]["simulationTime"]["broadcastGossip"]["algorithm"] = "BroadcastGossip";
        }

        // Export the edgelist
        upcxx::barrier();
        if (!programOptions.exportPath.empty()) {
            graph.ExportIntoFile(programOptions.exportPath);
        }
    }
    else if (programOptions.model == "knodel") {
        using VertexData = long unsigned int;
        using EdgeData = long unsigned int;

        if (upcxx::rank_me() == 0) {
            resultJson["model"] = "knodel";
        }

        // Initialize the graph
        upcxx::barrier();
        PGASGraph::Graph<PGASGraph::Generators::Knodel::Vertex<VertexData, EdgeData>,
            PGASGraph::Generators::Knodel::Edge<EdgeData>>
            graph(programOptions.totalNumberVertices, verticesPerRank);

        upcxx::barrier();

        // Generate edges/vertices
        auto start = std::chrono::steady_clock::now();
        const std::size_t delta{ static_cast<std::size_t>(std::log2(programOptions.totalNumberVertices)) };
        const auto edgeCount{PGASGraph::Generators::Knodel::Generate(programOptions.totalNumberVertices, verticesPerRank, delta, graph)};
        auto end = std::chrono::steady_clock::now();
        auto took = std::chrono::duration<double>(end - start).count();

        // Collect results
        resultJson["counters"]["generationTime"][fmt::format("rank_{}", upcxx::rank_me())] = took;
        resultJson["counters"]["edgeCount"][fmt::format("rank_{}", upcxx::rank_me())] = edgeCount;

        upcxx::barrier();
        start = std::chrono::steady_clock::now();
        //if (programOptions.algorithm == "randomizedpushgossip")
        {
            PGASGraph::Generators::Knodel::VertexId vertexId(0, 0, 0);
            PGASGraph::Algorithms::Gossip::PushRandomizedGossip(graph, vertexId, 15);
        }
        end = std::chrono::steady_clock::now();
        took = std::chrono::duration<double>(end - start).count();
        resultJson["counters"]["simulationTime"]["randomizedPushGossip"][fmt::format("rank_{}", upcxx::rank_me())] = took;
        if (upcxx::rank_me() == 0) {
            resultJson["counters"]["simulationTime"]["randomizedPushGossip"]["algorithm"] = "RandomizedPushGossip";
        }

        upcxx::barrier();
        start = std::chrono::steady_clock::now();
        //if (programOptions.algorithm == "randomizedpushgossip")
        //else if (programOptions.algorithm == "broadcastgossip")
        {
            if (upcxx::rank_me() == 0) {
                resultJson["algorithm"] = "broadcastGossip";
            }

            PGASGraph::Generators::Knodel::VertexId vertexId(0, 0, 0);
            PGASGraph::Algorithms::Gossip::BroadcastGossip(graph, vertexId, 15);
        }
        end = std::chrono::steady_clock::now();
        took = std::chrono::duration<double>(end - start).count();
        resultJson["counters"]["simulationTime"]["broadcastGossip"][fmt::format("rank_{}", upcxx::rank_me())] = took;
        if (upcxx::rank_me() == 0) {
            resultJson["counters"]["simulationTime"]["broadcastGossip"]["algorithm"] = "BroadcastGossip";
        }

        //else {
        //    {
        //        std::stringstream ss;
        //        ss << "Following algorithm=" << programOptions.algorithm << " is not supported by PGASGraph" << std::endl;
        //        PGASGraph::logMsg(ss.str());
        //    }
        //    return -1;
        //}
        // Export the edgelist
        upcxx::barrier();
        if (!programOptions.exportPath.empty()) {
            graph.ExportIntoFile(programOptions.exportPath);
        }
    }
    else {
        {
            std::stringstream ss;
            ss << "Following model=" << programOptions.model << " is not supported by PGASGraph" << std::endl;
            //PGASGraph::logMsg(ss.str());
        }
        return -1;
    }

    if (0 == upcxx::rank_me())
    {
        timeNow = std::time(nullptr);
        dt = std::ctime(&timeNow);
        resultJson["finishTime"] = currentTime();
        // PGASGraph::logMsg("Finish time: " + std::string(dt));
        // PGASGraph::logMsg("******************************************");

        for (upcxx::intrank_t r = 1; r < upcxx::rank_n(); r++)
        {
            auto rResult = upcxx::rpc(r, [&resultJson]() { return resultJson.dump(); }).wait();
            auto rResultJson = nlohmann::json::parse(rResult);
            resultJson.merge_patch(rResultJson);
        }

        resultJson["nodes"] = upcxx::rank_n();

        double generationMean{ 0 };
        generationMean += resultJson["counters"]["generationTime"][fmt::format("rank_{}", 0)].get<double>();
        for (upcxx::intrank_t r = 1; r < upcxx::rank_n(); r++)
        {
            generationMean = generationMean + resultJson["counters"]["generationTime"][fmt::format("rank_{}", r)].get<double>();
        }
        generationMean /= upcxx::rank_n();
        resultJson["counters"]["generationTime"]["mean"] = generationMean;

        double simulationMean{ 0 };
        simulationMean += resultJson["counters"]["simulationTime"]["randomizedPushGossip"][fmt::format("rank_{}", 0)].get<double>();
        for (upcxx::intrank_t r = 1; r < upcxx::rank_n(); r++)
        {
            generationMean = generationMean + resultJson["counters"]["simulationTime"]["randomizedPushGossip"][fmt::format("rank_{}", r)].get<double>();
        }
        generationMean /= upcxx::rank_n();
        resultJson["counters"]["simulationTime"]["randomizedPushGossip"]["mean"] = generationMean;

        simulationMean = 0;
        simulationMean += resultJson["counters"]["simulationTime"]["broadcastGossip"][fmt::format("rank_{}", 0)].get<double>();
        for (upcxx::intrank_t r = 1; r < upcxx::rank_n(); r++)
        {
            generationMean = generationMean + resultJson["counters"]["simulationTime"]["broadcastGossip"][fmt::format("rank_{}", r)].get<double>();
        }
        generationMean /= upcxx::rank_n();
        resultJson["counters"]["simulationTime"]["broadcastGossip"]["mean"] = generationMean;

        std::size_t totalEdgeCount{0};
        totalEdgeCount += resultJson["counters"]["edgeCount"][fmt::format("rank_{}", 0)].get<double>();
        for (upcxx::intrank_t r = 1; r < upcxx::rank_n(); r++)
        {
            totalEdgeCount = totalEdgeCount + resultJson["counters"]["edgeCount"][fmt::format("rank_{}", r)].get<std::size_t>();
        }
        resultJson["counters"]["edgeCount"]["total"] = totalEdgeCount;

        std::cout << resultJson << std::endl;

        if (!programOptions.metricsJsonPath.empty()) {
            std::fstream metricsJsonStream(programOptions.metricsJsonPath, std::ios::trunc | std::ios::in | std::ios::out);
            if (!metricsJsonStream.is_open()) {
                {

                    std::stringstream ss;
                    ss << "Unable to open metrics JSON file=" << programOptions.metricsJsonPath;
                    //PGASGraph::logMsg(ss.str());
                }
            }
            else {
                metricsJsonStream << resultJson << std::endl;
            }
        }
    }

    upcxx::barrier();
    upcxx::finalize();
    return 0;
};
