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
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/erdos_renyi_generator.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/small_world_generator.hpp>
#include <boost/program_options.hpp>
#include <boost/random/linear_congruential.hpp>

// Usings
namespace po = boost::program_options;

po::options_description createProgramOptions() {
  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")(
      "vertex-count", po::value<int>(),
      "set vertex count")("degree", po::value<int>(), "set degree")(
      "percentage", po::value<double>(), "set connectivity percentage");
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
  using BoostGraph =
      boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
                            MyBundledVertex, MyBundledEdge>;
  using SWGen = boost::small_world_iterator<boost::minstd_rand, BoostGraph>;

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

  // Get vertex count.
  size_t degree = 10;
  if (vm.count("degree")) {
    degree = vm["degree"].as<int>();
  }

  // Get vertex count.
  double percentage = 5.0;
  if (vm.count("percentage")) {
    percentage = vm["percentage"].as<double>();
  }

  // Distribute vertices over the ranks.
  const size_t verticesPerRank =
      (totalNumberVertices + upcxx::rank_n() - 1) / upcxx::rank_n();

  PGASGraph::Graph<std::string, int> pgasGraph(totalNumberVertices,
                                               verticesPerRank);

  if (0 == upcxx::rank_me()) {
    // Generate the graph.
    auto start = std::chrono::steady_clock::now();
    boost::minstd_rand gen;

    BoostGraph boostGraph{GraphUtilities::generateRandomConnectedGraph<BoostGraph>(
        totalNumberVertices, percentage)};

    // Assign ids to the vertices.
    set_my_bundled_vertexes(boostGraph);

    // Assign weights to the edges.
    auto edges = boost::edges(boostGraph);
    int weight{0};
    for (auto it = edges.first; it != edges.second; ++it) {
      boostGraph[*it].first = weight++;
    }

    auto vs = boost::vertices(boostGraph);
    weight = 0;
    for (auto vit = vs.first; vit != vs.second; ++vit) {
      auto neighbors = boost::adjacent_vertices(*vit, boostGraph);
      for (auto nit = neighbors.first; nit != neighbors.second; ++nit) {
        pgasGraph.AddEdge({boostGraph[*vit].id, boostGraph[*nit].id, weight++});
        if (boostGraph[*vit].id == boostGraph[*nit].id) {
          PGASGraph::logMsg("equal");
          upcxx::finalize();
          return 0;
        }
      }
    }

    auto end = std::chrono::steady_clock::now();

    PGASGraph::logMsg("*********");
    PGASGraph::logMsg(
        "Graph generation elapsed time: " +
        std::to_string(std::chrono::duration<double>(end - start).count()));

    PGASGraph::logMsg("Vertex count: " + std::to_string(totalNumberVertices));

    PGASGraph::logMsg("Vertex count per rank: " +
                      std::to_string(verticesPerRank));

    PGASGraph::logMsg("Edge count: " +
                      std::to_string(boost::num_edges(boostGraph)));

    PGASGraph::logMsg("Connectivity percentage: " + std::to_string(percentage));

    std::vector<int> components(boost::num_vertices(boostGraph));
    int numComponents{boost::connected_components(boostGraph, &components[0])};
    PGASGraph::logMsg("Number of connected components: " +
                      std::to_string(numComponents));

    PGASGraph::logMsg("UPCXX: Number of Ranks: " +
                      std::to_string(upcxx::rank_n()));
  }

  upcxx::barrier();

  auto start = std::chrono::steady_clock::now();
  auto mst = pgasGraph.MST();
  auto end = std::chrono::steady_clock::now();

  upcxx::barrier();
  // if (0 == upcxx::rank_me()) {
  PGASGraph::logMsg(
      "MST Elapsed time: " +
      std::to_string(std::chrono::duration<double>(end - start).count()));
  // }

  if (0 == upcxx::rank_me()) {
    upcxx::rpc(
        0,
        [](decltype(mst) &localMst) {
          size_t cost{0};
          auto begin = localMst->begin();
          for (; begin != localMst->end(); ++begin) {
            cost += begin->first;
          }
          PGASGraph::logMsg("MST Cost: " + std::to_string(cost));
        },
        mst)
	    .wait();

    size_t peakSize = getPeakRSS( );
    PGASGraph::logMsg("Peak Mem Usage: " + std::to_string(peakSize/1000000) + "MB");
  }
  PGASGraph::logMsg("*********");

  upcxx::barrier();
  return 0;

  upcxx::finalize();
  return 0;
};
