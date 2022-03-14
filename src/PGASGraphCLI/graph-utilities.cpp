// PGASGraph
#include <pgas-graph/graph-utilities.hpp>
#include <pgas-graph/pgas-graph.h>

// STL
#include <list>

namespace GraphUtilities {

int listAt(const std::list<int> &l, size_t idx) {
  if (idx < 0 || idx >= l.size()) {
    return -1;
  }

  auto begin{l.begin()};
  while (idx != 0) {
    ++begin;
    --idx;
  }

  return *begin;
}

// BoostGraph generateRandomConnectedGraph(const size_t vertexCount,
//                                         double percentage) {
//   std::list<int> unconnected;
//   std::vector<int> connected;
//   connected.reserve(vertexCount);
//
//   connected.push_back(1);
//   for (int i = 2; i <= vertexCount; ++i) {
//     unconnected.push_back(i);
//   }
//
//   size_t extraEdges{
//       static_cast<size_t>((vertexCount * vertexCount / 2) * (percentage /
//       100.0))};
//   size_t edges{0};
//
//   std::random_device rnd;
//   std::mt19937 gen{rnd()};
//   std::uniform_int_distribution<int> dist(1, vertexCount);
//   BoostGraph boostGraph;
//
//   while (!unconnected.empty()) {
//     int temp1 = std::uniform_int_distribution<int>(0, connected.size())(gen);
//     int temp2 =
//         std::uniform_int_distribution<int>(0, unconnected.size() - 1)(gen);
//
//     int u{connected[temp1]};
//     auto unconnectedItem{listAt(unconnected, temp2)};
//     assert(unconnectedItem != -1);
//     int v{unconnectedItem};
//     if (u != v) {
//       boost::add_edge(u, v, boostGraph);
//     }
//
//     unconnected.erase(
//         std::find(unconnected.begin(), unconnected.end(), unconnectedItem));
//
//     connected.push_back(v);
//   }
//
//   while (extraEdges != 0) {
//     int temp1 = std::uniform_int_distribution<int>(1, vertexCount)(gen);
//     int temp2 = std::uniform_int_distribution<int>(1, vertexCount)(gen);
//     if (temp1 != temp2) {
//       boost::add_edge(temp1, temp2, boostGraph);
//       extraEdges--;
//     }
//   }
//
//   return boostGraph;
// }

// po::options_description createProgramOptions() {
//   po::options_description desc("Allowed options");
//   desc.add_options()("help", "produce help message")(
//       "vertex-count", po::value<int>(),
//       "set vertex count")("degree", po::value<int>(), "degree");
//   return desc;
// }

// GraphType generateSmallWorld(int size, int degree) {
//   boost::minstd_rand gen;
//   GraphType g(size);
//   for (auto it = SWGen(gen, size, degree, 0.1); it != SWGen(); ++it) {
//     if (!boost::edge(it->first, it->second, g).second &&
//         it->first != it->second) {
//       boost::add_edge(it->first, it->second, g);
//     }
//   }

//   return g;
// }

// void convertBoostGraphToPGASGraph(
// const GraphType &boostGraph,
// PGASGraph::Graph<std::string, size_t> &pgasGraph) {}

} // namespace GraphUtilities
