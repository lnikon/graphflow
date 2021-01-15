#include <pgas-graph/pgas-graph.h>

#include <iostream>

int main() {
  upcxx::init();
  PGASGraph::Graph<std::string, int> g;

  if (upcxx::rank_me() == 0) {
  } else if (upcxx::rank_me() == 1) {
    g.AddEdge({1, 2, 3});
    g.AddEdge({2, 3, 5});
  } else if (upcxx::rank_me() == 2) {
    g.AddEdge({3, 4, 9});
  } else if (upcxx::rank_me() == 3) {
    g.AddEdge({4, 1, 12});
  }

  // std::cout << std::boolalpha << "g.HasEdge(1, 2) = " << g.HasEdge(1, 2) << '\n'
  //           << "g.HasEdge(2, 3) = " << g.HasEdge(2, 3) << '\n'
  //           << "g.HasEdge(3, 4) = " << g.HasEdge(3, 4) << '\n'
  //           << "g.HasEdge(4, 1) = " << g.HasEdge(4, 1) << '\n'
  //           << "g.HasEdge(5, 9) = " << g.HasEdge(5, 9) << '\n'
  //           << "g.HasEdge(4, 7) = " << g.HasEdge(4, 7) << '\n';
  //

  g.MST();

  upcxx::finalize();
  return 0;
};
