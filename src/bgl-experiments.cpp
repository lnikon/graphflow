// PGASGraph
#include <pgas-graph/graph-utilities.hpp>
#include <pgas-graph/pgas-graph.h>

// STL
#include <chrono>
#include <iosfwd>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <string>

// Boost
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/erdos_renyi_generator.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/small_world_generator.hpp>
#include <boost/program_options.hpp>
#include <boost/random/linear_congruential.hpp>

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

template <typename Graph, typename BundledVertex>
typename boost::graph_traits<Graph>::vertex_descriptor
add_bundled_vertex(const BundledVertex &v, Graph &g) noexcept {
  static_assert(!std::is_const<Graph>::value, "graph cannot be const");
  const auto vd = boost::add_vertex(g);
  g[vd] = v;
  return vd;
}

template <typename Graph> void set_my_bundled_vertexes(Graph &g) {
  static_assert(!std::is_const<Graph>::value, "graph cannot be const");
  const auto vip = boost::vertices(g);
  const auto j = vip.second;

  PGASGraph::Id id{0};
  for (auto i = vip.first; i != j; ++i) {
    g[*i] = MyBundledVertex(id++);
  }
}

template <typename Graph, typename Function>
void for_each_bundled_vertex(Graph &g, Function f) {
  static_assert(!std::is_const<Graph>::value, "graph cannot be const");
  const auto vip = boost::vertices(g);
  const auto j = vip.second;

  PGASGraph::Id id{0};
  for (auto i = vip.first; i != j; ++i) {
    f(g[*i]);
  }
}

int main(int argc, char *argv[]) {
  size_t totalNumberVertices{200};
  GraphUtilities::BoostGraph boostGraph{
      GraphUtilities::generateRandomConnectedGraph(totalNumberVertices, 30.0)};

  // upcxx::init();
  // using GT = boost::adjacency_list<boost::vecS, boost::vecS,
  // boost::undirectedS,
  //                                  MyBundledVertex, MyBundledEdge>;
  // using ERGen = boost::erdos_renyi_iterator<boost::minstd_rand, GT>;

  // // Generate the graph.
  // boost::minstd_rand gen;
  // const size_t vertexCount{63};
  // GT g(ERGen(gen, vertexCount, 0.05), ERGen(), 100);

  // // Assign ids to the vertices.
  // set_my_bundled_vertexes(g);

  // // Assign weights to the edges.
  // auto edges = boost::edges(g);
  // int weight{0};
  // for (auto it = edges.first; it != edges.second; ++it) {
  //   g[*it].first = weight++;
  // }

  // // Convert to the PGASGraph.
  // const auto verticesPerRank{(vertexCount + upcxx::rank_n() - 1) /
  //                            upcxx::rank_n()};
  // PGASGraph::logMsg("vertexCount=" + std::to_string(vertexCount));
  // PGASGraph::logMsg("verticesPerRank=" + std::to_string(verticesPerRank));
  // PGASGraph::Graph<std::string, int> pgasGraph(vertexCount, verticesPerRank);
  // auto vs = boost::vertices(g);
  // weight = 0;
  // for (auto vit = vs.first; vit != vs.second; ++vit) {
  //   auto neighbors = boost::adjacent_vertices(*vit, g);
  //   for (auto nit = neighbors.first; nit != neighbors.second; ++nit) {
  //     pgasGraph.AddEdge({g[*vit].id, g[*nit].id, weight++});
  //   }
  // }

  // // auto start = std::chrono::steady_clock::now();
  // // pgasGraph.MST();
  // // auto end = std::chrono::steady_clock::now();

  // // PGASGraph::logMsg(
  // //     "MST Elapsed time: " +
  // //     std::to_string(std::chrono::duration<double>(end - start).count()));

  // upcxx::finalize();
  return 0;
};
