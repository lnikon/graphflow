#include <pgas-graph/pgas-graph.h>

#include <iostream>

int main() {
  upcxx::init();

  const size_t totalNumberVertices = 5;
  const size_t verticesPerRank =
      (totalNumberVertices + upcxx::rank_n() - 1) / upcxx::rank_n();

  PGASGraph::Graph<std::string, int> g(totalNumberVertices, verticesPerRank);
  g.AddEdge({0, 1, 3});
  g.AddEdge({2, 3, 9});
  g.AddEdge({1, 2, 5});
  g.AddEdge({3, 0, 12});

  upcxx::barrier();

  // std::cout << std::boolalpha << "g.HasEdge(1, 2) = " << g.HasEdge(1, 2) <<
  // '\n'
  //           << "g.HasEdge(2, 3) = " << g.HasEdge(2, 3) << '\n'
  //           << "g.HasEdge(3, 4) = " << g.HasEdge(3, 4) << '\n'
  //           << "g.HasEdge(4, 1) = " << g.HasEdge(4, 1) << '\n'
  //           << "g.HasEdge(5, 9) = " << g.HasEdge(5, 9) << '\n'
  //           << "g.HasEdge(4, 7) = " << g.HasEdge(4, 7) << '\n';

  g.printLocal();

  upcxx::barrier();
  if (0 == upcxx::rank_me()) {
    std::cout << " ************** \n";
  }
  g.MST();

  upcxx::finalize();
  return 0;
};
