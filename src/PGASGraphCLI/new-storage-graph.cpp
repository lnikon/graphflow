// PGASGraph
// #include <performance/performance_monitor.hpp>
#include <pgas-graph/pgas-graph.h>

// CopyPasted
#include <pgas-graph/cppmemusage.hpp>

// STL
#include <chrono>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <random>
#include <cmath>
#include <limits>

// Boost
#include <boost/program_options.hpp>
#include <fmt/core.h>
#include <nlohmann/json.hpp>

// System
#include <unistd.h>

// Usings
namespace po = boost::program_options;

// TODO: Move into separate module
auto currentTime()
{
    auto time = std::time(nullptr);
    return std::asctime(std::localtime(&time));
}

struct ProgramOptions
{
    size_t totalNumberVertices{256};
    double percentage{5.0};
    bool printLocal{false};
    std::string exportPath{};
    bool usePapi{false};
};

po::options_description CreateProgramOptions()
{
    po::options_description desc("Allowed options");
    desc.add_options()("help", "produce help message");
    desc.add_options()("vertex-count", po::value<int>(), "set vertex count");
    desc.add_options()("percentage", po::value<double>(), "set connectivity percentage");
    desc.add_options()("print-local", po::value<int>(), "print edges on the current node");
    desc.add_options()(
        "export-path", po::value<std::string>(), "export graph as a edgelist into the file");
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

    // Run PAPI or not.
    if (vm.count("use-papi"))
    {
        result.usePapi = vm["use-papi"].as<bool>();
    }

    return result;
}

using MyBundledVertex = PGASGraph::Graph<std::string, size_t>::Vertex;
using MyBundledEdge = PGASGraph::Graph<std::string, size_t>::Vertex::weight_node_t;

std::ostream& operator<<(std::ostream& os, const MyBundledVertex& e) noexcept
{
    os << e.id;
    return os;
}

std::ostream& operator<<(std::ostream& os, const MyBundledEdge& e) noexcept
{
    os << e.first;
    return os;
}

bool operator==(const MyBundledVertex& lhs, const MyBundledVertex& rhs) noexcept
{
    return lhs.id == rhs.id;
}

bool operator!=(const MyBundledVertex& lhs, const MyBundledVertex& rhs) noexcept
{
    return !(lhs == rhs);
}

using PGASGraphType = PGASGraph::Graph<std::string, size_t>;
size_t
generateRandomConnectedPGASGraph(const size_t vertexCount, double percentage, PGASGraphType& graph)
{
    using PId = PGASGraph::Id;
    const auto rank_me{upcxx::rank_me()};
    const auto rank_n{upcxx::rank_n()};
    std::list<PId> unconnected;
    std::vector<PId> connected;
    connected.reserve(vertexCount);

    size_t edges{0};

    std::random_device rnd;
    std::mt19937 gen{rnd()};

    const auto minId = rank_me * vertexCount;
    const auto maxId = (rank_me + 1) * vertexCount - 1;

    connected.push_back(minId);
    for (auto i = minId + 1; i <= maxId; ++i)
    {
        unconnected.push_back(i);
    }

    using dist_weight_t = upcxx::dist_object<size_t>;
    dist_weight_t weight{1};
    auto genWeight = [](dist_weight_t& weight)
    {
        return upcxx::rpc(
                   0,
                   [](dist_weight_t& weight)
                   {
                       auto res = *weight;
                       *weight += 1;
                       return res;
                   },
                   weight)
            .wait();
    };

    auto start = std::chrono::steady_clock::now();
    while (!unconnected.empty())
    {
        const auto temp1 =
            std::uniform_int_distribution<unsigned long long>(0, connected.size())(gen);
        const auto temp2 =
            std::uniform_int_distribution<unsigned long long>(0, unconnected.size() - 1)(gen);

        PGASGraph::Id u{connected[temp1]};
        if (u < minId || u > maxId)
        {
            continue;
        }

        auto unconnectedItemIt{unconnected.begin()};
        std::advance(unconnectedItemIt, temp2);
        assert(unconnectedItemIt != unconnected.end());
        auto unconnectedItem{*unconnectedItemIt};
        PId v{unconnectedItem};
        if (v < minId || v > maxId)
        {
            continue;
        }

        if (u != v)
        {
            if (graph.AddEdge({u, v, genWeight(weight)}))
            {
                edges++;
            }
        }

        unconnected.erase(unconnectedItemIt);
        connected.push_back(v);
    }

    auto end = std::chrono::steady_clock::now();
    // PGASGraph::logMsg("Connected component core generation time: " +
    //                   std::to_string(std::chrono::duration<double>(end - start).count()));

    const size_t extraEdges{static_cast<size_t>(
        static_cast<double>((vertexCount * vertexCount) / 2) * (percentage / 100.0))};

    start = std::chrono::steady_clock::now();
    auto copyExtraEdges{extraEdges};
    size_t edgesInsideCurrentComponent{0};
    while (copyExtraEdges != 0)
    {
        PId u = std::uniform_int_distribution<unsigned long long>(minId, maxId)(gen);
        PId v = std::uniform_int_distribution<unsigned long long>(minId, maxId)(gen);
        if (u != v)
        {
            if (graph.AddEdge({u, v, genWeight(weight)}))
            {
                edgesInsideCurrentComponent++;
                edges++;
            }
            --copyExtraEdges;
        }
    }
    end = std::chrono::steady_clock::now();
    // PGASGraph::logMsg("Connected component interior generation time: " +
    //                   std::to_string(std::chrono::duration<double>(end - start).count()));
    // PGASGraph::logMsg("Edges inside current component: " +
    //                   std::to_string(edgesInsideCurrentComponent));

    auto genIdForPartition = [&gen](const auto rank, const auto vertexCount)
    {
        auto minId = rank * vertexCount;
        auto maxId = (rank + 1) * vertexCount - 1;
        return std::uniform_int_distribution<PId>(minId, maxId)(gen);
    };

    start = std::chrono::steady_clock::now();
    size_t edgesWithOtherComponents{0};
    for (PGASGraph::Rank r1 = 0; r1 < rank_n - 1; ++r1)
    {
        for (PGASGraph::Rank r2 = r1 + 1; r2 < rank_n; ++r2)
        {
            copyExtraEdges = rank_n + extraEdges;
            while (copyExtraEdges != 0)
            {
                const auto idR1{genIdForPartition(r1, vertexCount)};
                const auto idR2{genIdForPartition(r2, vertexCount)};
                if (idR1 != idR2)
                {
                    if (graph.AddEdge({idR1, idR2, genWeight(weight)}))
                    {
                        edgesWithOtherComponents++;
                        edges++;
                    }
                    --copyExtraEdges;
                }
            }
        }
    }
    end = std::chrono::steady_clock::now();
    // PGASGraph::logMsg("Connected component exterior generation time: " +
    //                   std::to_string(std::chrono::duration<double>(end - start).count()));
    // PGASGraph::logMsg("Edges with other components: " +
    // std::to_string(edgesWithOtherComponents));
    upcxx::barrier();
    return edges;
}

struct UPCXXInitializer
{
    UPCXXInitializer()
    {
        upcxx::init();
    }

    ~UPCXXInitializer()
    {
        upcxx::finalize();
    }
};

using PGASGraphType = PGASGraph::Graph<std::string, size_t>;
size_t generateKnodelGraph(const size_t vertexCount, const size_t delta, PGASGraphType& graph)
{
    size_t edgeCount{0};
    for (std::uint64_t i = 0; i < 2; ++i)
    {
        for (std::uint64_t j = 0; j < vertexCount / 2; ++j)
        {
            for (std::uint64_t d = 1; d <= delta; ++d)
            {
                const auto from{i};
                const auto to{(std::uint64_t)(j + std::pow(2, d) - 1) % (vertexCount)};
                if (graph.AddEdge({from, to, 0}))
                {
                    edgeCount++;
                }
            }
        }
    }

    return edgeCount;
}

int main(int argc, char* argv[])
{
    UPCXXInitializer upcxxInitializer{};

    nlohmann::json resultJson;

    std::time_t timeNow = std::time(nullptr);
    char* dt = nullptr;
    if (0 == upcxx::rank_me())
    {
        // PGASGraph::logMsg("******************************************");
        // PGASGraph::logMsg("Start time: " + std::string(dt));
        resultJson["start-time"] = currentTime();
    }

    // Print hostname to make sure that the run is truly distributed.
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    // upcxx::rpc(0, [hostname]() { PGASGraph::logMsg(std::string{"Hostname: "} + hostname);
    // }).wait();

    upcxx::barrier();

    const auto programOptions{ParseOptions(argc, argv)};
    if (upcxx::rank_me() == 0)
    {
        // PGASGraph::logMsg("Total number of vertices: " +
        //                   std::to_string(programOptions.totalNumberVertices));
        // PGASGraph::logMsg("Percentage: " + std::to_string(programOptions.percentage));
        resultJson["total-vertex-count"] = std::to_string(programOptions.totalNumberVertices);
        resultJson["percentage"] = std::to_string(programOptions.percentage);
    }

    // Distribute vertices over the ranks.
    const size_t verticesPerRank =
        (programOptions.totalNumberVertices + upcxx::rank_n() - 1) / upcxx::rank_n();

    PGASGraphType pgasGraph(programOptions.totalNumberVertices, verticesPerRank);
    const auto rank_n = upcxx::rank_n();
    // for (auto r = decltype(rank_n){0}; r < rank_n; ++r)
    {
        if (0 == 0 /* upcxx::rank_me() */)
        {
            auto start = std::chrono::steady_clock::now();
            // auto edgeCount = generateRandomConnectedPGASGraph(
            //     verticesPerRank, programOptions.percentage, pgasGraph);
            auto edgeCount = generateKnodelGraph(programOptions.totalNumberVertices,
                                                 std::log2(programOptions.totalNumberVertices),
                                                 pgasGraph);
            auto end = std::chrono::steady_clock::now();
            auto took = std::chrono::duration<double>(end - start).count();

            // PGASGraph::logMsg("Graph generation elapsed time: " +
            //                   std::to_string(took));
            //  PGASGraph::logMsg("Edge count: " + std::to_string(edgeCount));
            //  PGASGraph::logMsg("Peak Mem Usage: " + std::to_string(peakSize / 1000000) + "MB");

            const size_t peakSize = getPeakRSS();
            // resultJson["counters"]["generation-time"][fmt::format("rank_{}", r)] = took;
            // resultJson["counters"]["edge-count"][fmt::format("rank_{}", r)] = edgeCount;
            // resultJson["counters"]["peak-rss"][fmt::format("rank_{}", r)] = peakSize;
        }
    }

    upcxx::barrier();

    // std::unique_ptr<Performance::PerformanceMonitor> perfMonitor{nullptr};
    // if (programOptions.usePapi)
    // {
    //     perfMonitor = std::make_unique<Performance::PerformanceMonitor>();
    // }

    auto start = std::chrono::steady_clock::now();

    // if (programOptions.usePapi)
    // {
    //     perfMonitor->Start();
    // }

    auto mstEdges = std::vector<int>{}; // pgasGraph.Kruskal();

    // if (programOptions.usePapi)
    // {
    //     perfMonitor->Finish();
    // }

    auto end = std::chrono::steady_clock::now();

    upcxx::barrier();

    const auto mstElapsedTime = std::chrono::duration<double>(end - start).count();
    // PGASGraph::logMsg("MST Elapsed Time: " +
    //                   std::to_string(mstElapsedTime));
    resultJson["computation"]["elapsed-time"][fmt::format("rank_{}", upcxx::rank_me())] =
        mstElapsedTime;

    if (upcxx::rank_me() == 0)
    {
        // const auto mstEdgeCount = upcxx::rpc(
        //                               upcxx::rank_me(),
        //                               [](decltype(mstEdges)& mstEdges)
        //                               {
        //                                   // PGASGraph::logMsg("MST Edges = " +
        //                                   // std::to_string(mstEdges->size()));
        //                                   return mstEdges->size();
        //                               },
        //                               mstEdges)
        //                               .wait();

        // resultJson["computation"]["edge-count"] = mstEdgeCount;

        // if (programOptions.usePapi)
        // {
        //     perfMonitor->Read();
        // }

        // if (programOptions.usePapi)
        // {
        //     perfMonitor->Report();
        // }

        // Testing things

        if (!programOptions.exportPath.empty())
        {
            // PGASGraph::logMsg("Exporting into \"" + programOptions.exportPath + "\"");
            pgasGraph.ExportIntoFile(programOptions.exportPath);
        }
    }

    if (programOptions.printLocal)
    {
        pgasGraph.printLocal();
    }

    upcxx::barrier();
    if (0 == upcxx::rank_me())
    {
        timeNow = std::time(nullptr);
        dt = std::ctime(&timeNow);
        resultJson["finish-time"] = currentTime();
        // PGASGraph::logMsg("Finish time: " + std::string(dt));
        // PGASGraph::logMsg("******************************************");

        for (upcxx::intrank_t r = 1; r < upcxx::rank_n(); r++)
        {
            auto rResult = upcxx::rpc(r, [&resultJson]() { return resultJson.dump(); }).wait();
            auto rResultJson = nlohmann::json::parse(rResult);
            resultJson.merge_patch(rResultJson);
        }

        std::cout << resultJson << std::endl;
    }
    upcxx::barrier();

    return 0;
}
