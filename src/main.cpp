#include <pgas-graph/pgas-graph.h>

#include <iostream>

int main() {
  upcxx::init();

  // const size_t totalNumberVertices = 5;
  const size_t totalNumberVertices = 8;
  const size_t verticesPerRank =
      (totalNumberVertices + upcxx::rank_n() - 1) / upcxx::rank_n();

  PGASGraph::Graph<std::string, int> g(totalNumberVertices, verticesPerRank);

  // from 0
  if (0 == upcxx::rank_me()) {
    g.AddEdge({0, 1, 12});
    g.AddEdge({0, 2, 11});
    g.AddEdge({0, 3, 10});
    g.AddEdge({0, 4, 9});
    g.AddEdge({0, 5, 8});
    g.AddEdge({0, 6, 7});

    // from 1
    g.AddEdge({1, 2, 6});
    g.AddEdge({1, 6, 5});

    // from 2
    g.AddEdge({2, 3, 4});

    // from 3
    g.AddEdge({3, 4, 3});

    // from 4
    g.AddEdge({4, 5, 2});

    // from 5
    g.AddEdge({5, 6, 1});
 

    // g.AddEdge({0, 1, 1});
    // g.AddEdge({0, 2, 2});
    // g.AddEdge({0, 3, 3});
    // g.AddEdge({0, 4, 4});
    // g.AddEdge({0, 5, 5});
    // g.AddEdge({0, 6, 6});

    // // from 1
    // g.AddEdge({1, 2, 7});
    // g.AddEdge({1, 6, 8});

    // // from 2
    // g.AddEdge({2, 3, 9});

    // // from 3
    // g.AddEdge({3, 4, 10});

    // // from 4
    // g.AddEdge({4, 5, 11});

    // // from 5
    // g.AddEdge({5, 6, 12});
  }

  // g.AddEdge({0, 1, 3});
  // g.AddEdge({2, 3, 9});
  // g.AddEdge({3, 0, 12});
  // g.AddEdge({1, 2, 5});

  upcxx::barrier();
  g.printLocal();
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
