// PGASGraph
#include <pgas-graph/pgas-graph.h>

// STL
#include <chrono>
#include <iostream>
#include <random>

// Boost
#include <boost/program_options.hpp>

// Usings
namespace po = boost::program_options;

po::options_description createProgramOptions() {
  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")(
      "vertex-count", po::value<int>(), "set vertex count");
  return desc;
}

int main(int argc, char *argv[]) {
  // Initialize UPCXX.
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

  // Distribute vertices over the ranks.
  const size_t verticesPerRank =
      (totalNumberVertices + upcxx::rank_n() - 1) / upcxx::rank_n();

  // Create graph.
  PGASGraph::Graph<std::string, size_t> g(totalNumberVertices, verticesPerRank);

  // Generate vertices.
  if (0 == upcxx::rank_me()) {
    std::random_device rnd;
    std::mt19937 generator(rnd());
    std::uniform_real_distribution<double> distribution(0, 1);

    size_t edgeCount{0};
    auto start = std::chrono::steady_clock::now();
    for (size_t i{1}; i < totalNumberVertices; ++i) {
      for (size_t j{1}; j < totalNumberVertices; ++j) {
        const auto diceRoll = distribution(generator);
        if (diceRoll >= 0.5) {
          g.AddEdge({i, j, i * j});
          edgeCount++;
        } else {
          PGASGraph::logMsg("dice roll: " + std::to_string(diceRoll));
        }
      }
    }

    auto end = std::chrono::steady_clock::now();
    PGASGraph::logMsg(
        "Graph generation elapsed time: " +
        std::to_string(std::chrono::duration<double>(end - start).count()) +
        "\n");

    PGASGraph::logMsg("Vertex count: " + std::to_string(totalNumberVertices));
    PGASGraph::logMsg("Vertex count per rank: " +
                      std::to_string(verticesPerRank));
    PGASGraph::logMsg("Edge count: " + std::to_string(edgeCount));
  }

  upcxx::barrier();

  auto start = std::chrono::steady_clock::now();
  g.MST();
  auto end = std::chrono::steady_clock::now();

  PGASGraph::logMsg(
      "MST Elapsed time: " +
      std::to_string(std::chrono::duration<double>(end - start).count()));

  upcxx::finalize();
  return 0;
};
