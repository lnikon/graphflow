// PGASGraph
#include <pgas-graph/graph-utilities.hpp>
#include <pgas-graph/pgas-graph.h>

// Usings
namespace po = boost::program_options;

namespace GraphUtilities {

po::options_description createProgramOptions() {
  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")(
      "vertex-count", po::value<int>(),
      "set vertex count")("degree", po::value<int>(), "degree");
  return desc;
}

GraphType generateSmallWorld(int size, int degree) {
  boost::minstd_rand gen;
  GraphType g(size);
  for (auto it = SWGen(gen, size, degree, 0.1); it != SWGen(); ++it) {
    if (!boost::edge(it->first, it->second, g).second &&
        it->first != it->second) {
      boost::add_edge(it->first, it->second, g);
    }
  }

  return g;
}

void convertBoostGraphToPGASGraph(
    const GraphType &boostGraph,
    PGASGraph::Graph<std::string, size_t> &pgasGraph) {}

} // namespace GraphUtilities
