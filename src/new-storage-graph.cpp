// PGASGraph
#include <pgas-graph/graph-utilities.hpp>
#include <pgas-graph/pgas-graph.h>

// CopyPasted
#include <pgas-graph/cppmemusage.hpp>

// STL
#include <chrono>
#include <iostream>
#include <random>

// Boost
#include <boost/program_options.hpp>

// System
#include <limits.h>
#include <unistd.h>

// Usings
namespace po = boost::program_options;

po::options_description createProgramOptions() {
  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")(
      "vertex-count", po::value<int>(),
      "set vertex count")("degree", po::value<int>(), "set degree")(
      "percentage", po::value<double>(), "set connectivity percentage")(
      "print-local", po::value<int>(), "print edges on the current node")(
      "export-path", po::value<std::string>(),
      "export graph as a edgelist into the file");
  return desc;
}

using MyBundledVertex = PGASGraph::Graph<std::string, size_t>::Vertex;
using MyBundledEdge =
    PGASGraph::Graph<std::string, size_t>::Vertex::weight_node_t;

std::ostream &operator<<(std::ostream &os, const MyBundledVertex &e) noexcept {
  os << e.id;
  return os;
}

std::ostream &operator<<(std::ostream &os, const MyBundledEdge &e) noexcept {
  os << e.first;
  return os;
}

bool operator==(const MyBundledVertex &lhs,
                const MyBundledVertex &rhs) noexcept {
  return lhs.id == rhs.id;
}

bool operator!=(const MyBundledVertex &lhs,
                const MyBundledVertex &rhs) noexcept {
  return !(lhs == rhs);
}

using PGASGraphType = PGASGraph::Graph<std::string, size_t>;
size_t generateRandomConnectedPGASGraph(const size_t vertexCount,
                                        double percentage,
                                        PGASGraphType &graph) {
  using PId = PGASGraph::Id;
  const auto rank_me{upcxx::rank_me()};
  const auto rank_n{upcxx::rank_n()};
  std::list<PId> unconnected;
  std::vector<PId> connected;
  connected.reserve(vertexCount);

  size_t extraEdges{static_cast<size_t>((vertexCount * vertexCount / 2) *
                                        (percentage / 100.0))};

  size_t edges{0};

  std::random_device rnd;
  std::mt19937 gen{rnd()};

  const auto minId = rank_me * vertexCount;
  const auto maxId = (rank_me + 1) * vertexCount - 1;

  connected.push_back(minId);
  for (auto i = minId + 1; i <= maxId; ++i) {
    unconnected.push_back(i);
  }

  PGASGraph::logMsg("min=" + std::to_string(minId) +
                    ", max=" + std::to_string((maxId)));

  using dist_weight_t = upcxx::dist_object<size_t>;
  dist_weight_t weight{1};

  // for (size_t vidx = minId; vidx < maxId; vidx++) {
  //     graph.AddEdge({vidx, vidx + 1, weight++});
  //     edges++;
  // }

  auto genWeight = [](dist_weight_t &weight) {
    return upcxx::rpc(
               0,
               [](dist_weight_t &weight) {
                 auto res = *weight;
                 *weight += 1;
                 return res;
               },
               weight)
        .wait();
  };

  while (!unconnected.empty()) {
    const int temp1 =
        std::uniform_int_distribution<int>(0, connected.size())(gen);
    const int temp2 =
        std::uniform_int_distribution<int>(0, unconnected.size() - 1)(gen);

    PGASGraph::Id u{connected[temp1]};
    if (u < minId || u > maxId) {
      continue;
    }

    auto unconnectedItemIt{unconnected.begin()};
    std::advance(unconnectedItemIt, temp2);
    assert(unconnectedItemIt != unconnected.end());
    auto unconnectedItem{*unconnectedItemIt};
    PId v{unconnectedItem};
    if (v < minId || v > maxId) {
      continue;
    }

    if (u != v) {
      graph.AddEdge({u, v, genWeight(weight)});
      edges++;
    }

    unconnected.erase(unconnectedItemIt);
    connected.push_back(v);
  }

  auto copyExtraEdges{extraEdges};
  while (copyExtraEdges != 0) {
    PId u = std::uniform_int_distribution<int>(minId, maxId)(gen);
    PId v = std::uniform_int_distribution<int>(minId, maxId)(gen);
    if (u != v) {
      graph.AddEdge({u, v, genWeight(weight)});
      edges++;
      copyExtraEdges--;
    }
  }

  auto genIdForPartition = [&gen](const auto rank, const auto vertexCount) {
    auto minId = rank * vertexCount;
    auto maxId = (rank + 1) * vertexCount - 1;
    return std::uniform_int_distribution<PId>(minId, maxId)(gen);
  };

  for (PGASGraph::Rank r1 = 0; r1 < rank_n - 1; ++r1) {
    for (PGASGraph::Rank r2 = r1 + 1; r2 < rank_n; ++r2) {
      copyExtraEdges = rank_n + vertexCount * 5 / 100;
      while (copyExtraEdges != 0) {
        const auto idR1{genIdForPartition(r1, vertexCount)};
        const auto idR2{genIdForPartition(r2, vertexCount)};
        if (idR1 != idR2) {
          graph.AddEdge({idR1, idR2, genWeight(weight)});
          edges++;
          --copyExtraEdges;
        }
      }
    }
  }

  upcxx::barrier();
  return edges;
}
int main(int argc, char *argv[]) {
  upcxx::init();

  // Parse command line options.
  auto optionsDesc = createProgramOptions();
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optionsDesc), vm);
  if (vm.count("help")) {
    std::cout << optionsDesc << std::endl;
    return 1;
  }

  // Get vertex count.
  size_t totalNumberVertices = 256;
  if (vm.count("vertex-count")) {
    totalNumberVertices = vm["vertex-count"].as<int>();
  }

  // Get vertex count.
  double percentage = 5.0;
  if (vm.count("percentage")) {
    percentage = vm["percentage"].as<double>();
  }

  // Get vertex count.
  bool printLocal = false;
  if (vm.count("print-local")) {
    printLocal = vm["print-local"].as<int>();
  }

  // Graph export path.
  std::string exportPath;
  if (vm.count("export-path")) {
    exportPath = vm["export-path"].as<std::string>();
  }

  // Distribute vertices over the ranks.
  const size_t verticesPerRank =
      (totalNumberVertices + upcxx::rank_n() - 1) / upcxx::rank_n();

  PGASGraphType pgasGraph(totalNumberVertices, verticesPerRank);
  const auto rank_n = upcxx::rank_n();
  for (auto r = decltype(rank_n){0}; r < rank_n; ++r) {
    if (r == upcxx::rank_me()) {
      auto start = std::chrono::steady_clock::now();
      auto edgeCount = generateRandomConnectedPGASGraph(verticesPerRank,
                                                        percentage, pgasGraph);
      auto end = std::chrono::steady_clock::now();
      PGASGraph::logMsg(
          "Graph generation elapsed time: " +
          std::to_string(std::chrono::duration<double>(end - start).count()));
      PGASGraph::logMsg("Edge count: " + std::to_string(edgeCount));
      size_t peakSize = getPeakRSS();
      PGASGraph::logMsg(
          "Peak Mem Usage: " + std::to_string(peakSize / 1000000) + "MB");
    }
  }

  upcxx::barrier();

  auto start = std::chrono::steady_clock::now();
  auto mstEdges = pgasGraph.Kruskal();
  auto end = std::chrono::steady_clock::now();

  upcxx::barrier();

  PGASGraph::logMsg(
      "MST Elapsed Time: " +
      std::to_string(std::chrono::duration<double>(end - start).count()));

  if (upcxx::rank_me() == 0) {
    upcxx::rpc(
        upcxx::rank_me(),
        [](decltype(mstEdges) &mstEdges) {
          PGASGraph::logMsg("MST Edges = " + std::to_string(mstEdges->size()));
        },
        mstEdges)
        .wait();
  }

  if (!exportPath.empty()) {
    pgasGraph.ExportIntoFile(exportPath);
  }

  if (printLocal) {
    pgasGraph.printLocal();
  }

  upcxx::finalize();
  return 0;
};
