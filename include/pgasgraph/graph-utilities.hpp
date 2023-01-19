#ifndef GRAPH_UTILITIES_HPP
#define GRAPH_UTILITIES_HPP

// PGASGraph
#include <pgas-graph/pgas-graph.h>

// STL
#include <chrono>
#include <iostream>
#include <random>

// Usings
namespace GraphUtilities {

using MyBundledVertex = PGASGraph::Graph<std::string, size_t>::Vertex;

using MyBundledEdge =
    PGASGraph::Graph<std::string, size_t>::Vertex::weight_node_t;

int listAt(const std::list<int> &l, size_t idx);

template <typename GraphType>
GraphType generateRandomConnectedGraph(const size_t vertexCount,
                                       double percentage) {
  std::list<int> unconnected;
  std::vector<int> connected;
  connected.reserve(vertexCount);

  connected.push_back(1);
  for (int i = 2; i <= vertexCount; ++i) {
    unconnected.push_back(i);
  }

  size_t extraEdges{static_cast<size_t>((vertexCount * vertexCount / 2) *
                                        (percentage / 100.0))};
  size_t edges{0};

  std::random_device rnd;
  std::mt19937 gen{rnd()};
  std::uniform_int_distribution<int> dist(1, vertexCount);
  GraphType boostGraph;

  while (!unconnected.empty()) {
    int temp1 = std::uniform_int_distribution<int>(0, connected.size())(gen);
    int temp2 =
        std::uniform_int_distribution<int>(0, unconnected.size() - 1)(gen);

    int u{connected[temp1]};
    auto unconnectedItemIt{unconnected.begin()};
    std::advance(unconnectedItemIt, temp2);

    assert(unconnectedItemIt != unconnected.end());
    auto unconnectedItem{*unconnectedItemIt};
    assert(unconnectedItem != -1);
    int v{unconnectedItem};
    if (u != v) {
      // boost::add_edge(u, v, boostGraph);
    }

    unconnected.erase(
        std::find(unconnected.begin(), unconnected.end(), unconnectedItem));
    connected.push_back(v);
  }

  while (extraEdges != 0) {
    int temp1 = std::uniform_int_distribution<int>(1, vertexCount)(gen);
    int temp2 = std::uniform_int_distribution<int>(1, vertexCount)(gen);
    if (temp1 != temp2) {
      // boost::add_edge(temp1, temp2, boostGraph);
      extraEdges--;
    }
  }

  return boostGraph;
}

// po::options_description createProgramOptions();

// GraphType generateSmallWorld(int size, int degree);

} // namespace GraphUtilities

#endif // GRAPH_UTILITIES_HPP
