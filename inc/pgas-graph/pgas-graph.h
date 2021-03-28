#ifndef PGAS_GRAPH_H
#define PGAS_GRAPH_H

// UPCXX
#include <upcxx/upcxx.hpp>

// STL
#include <fstream>
#include <queue>
#include <type_traits>
#include <unordered_map>

// Boost
#include <boost/timer/timer.hpp>

namespace PGASGraph {

#define DEBUG_LOG_MSG

void logMsg(const std::string &msg);

/*!
 * Type for the vertex Id.
 */
using Id = unsigned long long int;

/*!
 * Type for the vertex parent rank.
 */
using Rank = upcxx::intrank_t;

/*! \brief An adjacency list distributed graph.
 * Each vertex has assigned an Id which is unique across all the
 * vertices.
 */
template <typename VertexData, typename EdgeData> class Graph {
public:
  /*! \brief Represents edge in a graph.
   */
  struct Edge {
    /// Edge has two ends.
    Id from;
    Id to;

    /// User supplied data associated with each edge.
    EdgeData data;

    std::string ToString() {
      std::string result;
      result += "{from=";
      result += std::to_string(from);
      result += ", to=";
      result += std::to_string(to);
      result += ", weight=";
      result += std::to_string(data);
      result += "}\n";
      return result;
    }
  };

  /*! \brief Represents vertex in a graph.
   *
   * Each vertex has an unique id.
   */
  struct Vertex {
    /// A unique local id.
    Id id;

    /// A value stored in the vertex.
    VertexData value;

    /// A list of Id's of the neighbours.
    /// Id will be used to fetch actual pointer from the vertex store.
    using weight_node_t = std::pair<EdgeData, Id>;
    std::vector<weight_node_t> neighbours;

    Vertex() = default;
    Vertex(const Id id) : id(id) {}

    Id masterId() const { return upcxx::rank_me(); }

    std::string ToString() const {
      std::string result;
      result += "{id=" + std::to_string(id);
      result += "}";
      return result;
    }
  };

  Graph(const size_t totalNumberVertices, const size_t verticesPerRank);

  /*
   * Internal representations of the graph as a vertex store.
   * Currently vertex_store_type is used.
   *
   * @param vertex_store_type Map from the vertex Id to its global pointer.
   * @param graph_storage_type Type of the storage used for the graph.
   */
  using vertex_store_type = std::vector<upcxx::global_ptr<Vertex>>;
  using graph_storage_type = vertex_store_type;

  /*! \brief Adds an undirected edge between two nodes.
   *
   *  \param a First node.
   *  \param b Second node.
   */
  bool AddEdge(const Edge &edge);

  /*! \brief Check if two nodes are connected.
   *
   *  \param a First node.
   *  \param b Second node.
   */
  bool HasEdge(const Id &a, const Id &b) const;

  /*! \brief Return minimal id that is stored on the current rank.
   */
  Id minId() const {
      return upcxx::rank_me() * m_verticesPerRank;
  }

  /*! \brief Return maximal id that is stored on the current rank.
   */
  Id maxId() const {
      return (upcxx::rank_me() + 1) * m_verticesPerRank - 1;
  }

  void printLocal() {
    const auto& localVertexStore = m_vertexStore;
    std::string msg;
    for (auto id = size_t{0}; id < localVertexStore.size(); ++id) {
        msg += "\n" + std::to_string(id) + ": [";
        auto neighs = localVertexStore[id].local()->neighbours;
        for (const auto &neigh : neighs) {
          msg += " (" + std::to_string(neigh.first) + ", " +
                 std::to_string(neigh.second) + "), ";
        }
        msg += "]\n";
    }

    logMsg(msg);
  }

//  upcxx::dist_object<std::vector<std::pair<EdgeData, std::pair<Id, Id>>>>
//  MST() {
//    boost::timer::auto_cpu_timer t;

//    using added_set_t = upcxx::dist_object<std::unordered_set<Id>>;
//    using weight_node_t = typename Vertex::weight_node_t;

//    using local_edge_t = std::pair<EdgeData, std::pair<Id, Id>>;
//    auto ToString = [](local_edge_t edge) {
//      std::string result;
//      result += "{from=";
//      result += std::to_string(edge.second.first);
//      result += ", to=";
//      result += std::to_string(edge.second.second);
//      result += ", weight=";
//      result += std::to_string(edge.first);
//      result += "}\n";
//      return result;
//    };

//    using dist_local_edge_vec_t = upcxx::dist_object<std::vector<local_edge_t>>;
//    dist_local_edge_vec_t localMst{{}};

//    // Process with rank 0 is the master.
//    const Rank masterRank = 0;

//    auto addIdIntoMST = [this](added_set_t &addedSet, Id id) {
//      auto fut = upcxx::rpc(
//          getVertexParent(id),
//          [](added_set_t &addedSet, Id id) { addedSet->insert(id); }, addedSet,
//          id);
//      fut.wait();
//    };

//    auto isIdInMST = [this](added_set_t &addedSet, Id id) {
//      auto fut = upcxx::rpc(
//          getVertexParent(id),
//          [](added_set_t &addedSet, Id id) {
//            return addedSet->find(id) != addedSet->end();
//          },
//          addedSet, id);
//      bool res = fut.wait();
//      return res;
//    };

//    auto addedSetSize = [](added_set_t &addedSet) {
//      // TODO: Collect all added set sizes and terminate if equal to vertex
//      // count.
//      return upcxx::rpc(
//                 upcxx::rank_me(),
//                 [](added_set_t &addedSet) { return addedSet->size(); },
//                 addedSet)
//          .wait();
//    };

//    auto addEdgeIntoMst = [](dist_local_edge_vec_t &mstEdges,
//                             local_edge_t edge) {
//      upcxx::rpc(
//          0,
//          [](dist_local_edge_vec_t &mstEdges, local_edge_t edge) {
//            if (std::find_if(mstEdges->begin(), mstEdges->end(),
//                             [edge](const auto &current) {
//                               return edge.first == current.first &&
//                                      edge.second.first ==
//                                          current.second.first &&
//                                      edge.second.second ==
//                                          current.second.second;
//                             }) == mstEdges->end()) {
//              auto ToString = [](local_edge_t edge) {
//                std::string result;
//                result += "{from=";
//                result += std::to_string(edge.second.first);
//                result += ", to=";
//                result += std::to_string(edge.second.second);
//                result += ", weight=";
//                result += std::to_string(edge.first);
//                result += "}\n";
//                return result;
//              };

//              mstEdges->push_back(edge);
//            }
//          },
//          mstEdges, edge)
//          .wait();
//    };

//    std::unordered_map<Id, upcxx::global_ptr<Vertex>> localVertexStore =
//        m_vertexStore.fetch(upcxx::rank_me()).wait();
//    if (localVertexStore.empty()) {
//      logMsg("Empty local vertex store.");
//      return localMst;
//    }

//    // Each worker has its portion.
//    added_set_t addedSet{{}};

//    // Add INITIAL random vertex into MST.
//    bool isEmptyLocalVertexStore{false};
//    {
//      auto localVertexStore = m_vertexStore.fetch(upcxx::rank_me()).wait();
//      isEmptyLocalVertexStore = localVertexStore.empty();
//      if (isEmptyLocalVertexStore) {
//        return localMst;
//      }

//      const Id initialVertex = localVertexStore.begin()->second.local()->id;

//      addIdIntoMST(addedSet, initialVertex);
//      assert(localVertexStore[initialVertex].is_local());
//      Vertex *initialVertexPtr = localVertexStore[initialVertex].local();
//      local_edge_t minEdge =
//          std::make_pair(std::numeric_limits<int>::max(), std::make_pair(0, 0));
//      for (const auto &neigh : initialVertexPtr->neighbours) {
//        if (neigh.first < minEdge.first) {
//          minEdge.first = neigh.first;
//          minEdge.second.first = initialVertexPtr->id;
//          minEdge.second.second = neigh.second;
//        }
//      }

//      addEdgeIntoMst(localMst, minEdge);
//    }

//    auto isMSTFull = [this](dist_local_edge_vec_t &localMst) -> bool {
//      return upcxx::rpc(
//                 0,
//                 [](dist_local_edge_vec_t &localMst, size_t totalVertexCount) {
//                   if (localMst->empty()) {
//                     return false;
//                   }

//                   return localMst->size() == totalVertexCount - 1;
//                 },
//                 localMst, m_totalNumberVertices)
//          .wait();
//    };

//    auto getMSTSize = [this](dist_local_edge_vec_t &localMst) -> bool {
//      return upcxx::rpc(
//                 0,
//                 [](dist_local_edge_vec_t &localMst) {
//                   return localMst->size();
//                 },
//                 localMst)
//          .wait();
//    };

//    upcxx::barrier();

//    while (!isMSTFull(localMst) && !isEmptyLocalVertexStore) {
//      // Min edge in the current cut.
//      local_edge_t minEdge =
//          std::make_pair(std::numeric_limits<int>::max(), std::make_pair(0, 0));

//      // Find edge with minimum weight that connects vertex in the current
//      // cut to the MST.
//      for (const auto &[id, globalVertex] :
//           m_vertexStore.fetch(upcxx::rank_me()).wait()) {

//        assert(globalVertex.is_local());
//        Vertex *localVertex = globalVertex.local();

//        const bool isCurrentInMst = isIdInMST(addedSet, localVertex->id);
//        for (const auto &neigh : localVertex->neighbours) {
//          if (neigh.first < minEdge.first) {
//            if (isCurrentInMst && !isIdInMST(addedSet, neigh.second)) {
//              minEdge.first = neigh.first;
//              minEdge.second.first = localVertex->id;
//              minEdge.second.second = neigh.second;
//            } /* else if (!isCurrentInMst && isIdInMST(addedSet, neigh.second))
//            { minEdge.first = neigh.first; minEdge.second.first = neigh.second;
//              minEdge.second.second = localVertex->id;
//            } */
//          }
//        }
//      }

//      // TODO Can be removed?
//      // upcxx::barrier();
//      upcxx::barrier();

//      local_edge_t globalMinEdge =
//          upcxx::reduce_one(
//              minEdge,
//              [](const local_edge_t &lhs, const local_edge_t &rhs) {
//                return (lhs.first < rhs.first ? lhs : rhs);
//              },
//              0)
//              .wait();

//      if (globalMinEdge.first == std::numeric_limits<int>::max()) {
//        break;
//      }

//      upcxx::barrier();

//      if (0 == upcxx::rank_me()) {
//        if (globalMinEdge.first != std::numeric_limits<int>::max()) {
//          addIdIntoMST(addedSet, globalMinEdge.second.second);
//          addEdgeIntoMst(localMst, globalMinEdge);
//        }
//      }
//    }

    // if (0 == upcxx::rank_me()) {
    //   upcxx::rpc(
    //       0,
    //       [ToString](dist_local_edge_vec_t &localMst) {
    //         size_t cost{0};
    //         auto begin = localMst->begin();
    //         for (; begin != localMst->end(); ++begin) {
    //           cost += begin->first;
    //         }

    //         logMsg("MST Cost: " + std::to_string(cost));
    //       },
    //       localMst)
    //       .wait();
    // }

//    upcxx::barrier();
//    return localMst;
//  }

//  void ExportIntoFile(const std::string &fileName) const {
//    if (0 == upcxx::rank_me()) {
//      const auto &rank_n = upcxx::rank_n();

//      for (size_t r = 0; r < rank_n; ++r) {
//        upcxx::rpc(
//            r,
//            [](graph_storage_type &graph, std::string fileName) {
//              std::fstream stream(fileName, std::fstream::in |
//                                                std::fstream::out |
//                                                std::fstream::app);
//              if (!stream.is_open()) {
//                logMsg("Unable to open " + fileName + " to write into\n");
//                return;
//              }

//              for (const auto &[id, gptr] : *graph) {
//                Vertex *lptr = gptr.local();
//                for (auto &ngh : lptr->neighbours) {
//                  stream << "" << id << " " << ngh.second << "\n";
//                }
//              }
//            },
//            m_vertexStore, fileName)
//            .wait();
//      }
//    }
//  }

  Rank getVertexParent(const Id &a) const;

private:
  graph_storage_type m_vertexStore;
  const size_t m_totalNumberVertices;
  const size_t m_verticesPerRank;

  /*
   * Helper methods
   */
  bool addEdgeHelper(Edge edge);
  bool hasEdgeHelper(const Id &a, const Id &b) const;

  upcxx::global_ptr<Vertex> fetchVertexFromStore(const Id &id) const;
};

template <typename VertexData, typename EdgeData>
bool Graph<VertexData, EdgeData>::AddEdge(const Edge &edge) {
  // Loops are not allowed
  if (edge.from == edge.to) {
    return false;
  }

  return addEdgeHelper(edge);
}

template <typename VertexData, typename EdgeData>
Graph<VertexData, EdgeData>::Graph(const size_t totalNumberVertices,
                                   const size_t verticesPerRank)
    : m_vertexStore({}), m_totalNumberVertices(totalNumberVertices),
      m_verticesPerRank(verticesPerRank) {
//    upcxx::rpc(upcxx::rank_me(),
//               [](graph_storage_type& graph, size_t size) {
//                   graph->resize(size);
//               },
//               m_vertexStore,
//               verticesPerRank).wait();

    m_vertexStore.resize(verticesPerRank);
}

template <typename VertexData, typename EdgeData>
bool Graph<VertexData, EdgeData>::addEdgeHelper(Edge edge) {
//  // Get rank of the first vertex parent
//  const Rank &aRank = getVertexParent(edge.from);

//  // Get rank of the second vertex parent
//  const Rank &bRank = getVertexParent(edge.to);

//  auto parentRank = getVertexParent(edge.from);
//  auto singleEdgeInsertLambda = [](graph_storage_type &graph, Edge edge, Rank parentRank) {
//    size_t idx = edge.from - parentRank * graph->size();
//    graph->at(idx) = upcxx::new_<Vertex>(edge.from);
//    auto pLocalFrom = graph->at(idx).local();
//    pLocalFrom->neighbours.push_back({edge.data, edge.to});
//  };

//  upcxx::future<> aFuture =
//      upcxx::rpc(aRank, singleEdgeInsertLambda, m_vertexStore, edge, getVertexParent(edge.from));

//  upcxx::future<> bFuture =
//      upcxx::rpc(bRank, singleEdgeInsertLambda, m_vertexStore,
//                 Edge{edge.to, edge.from, edge.data}, getVertexParent(edge.to));

//  aFuture.wait();
//  bFuture.wait();

    auto insertSingleEdge = [this](Edge edge) {
        const auto parentRank = getVertexParent(edge.from);
        const size_t idx = edge.from - parentRank * m_vertexStore.size();
        m_vertexStore.at(idx) = upcxx::new_<Vertex>(edge.from);
        auto pLocalFrom = m_vertexStore.at(idx).local();
        pLocalFrom->neighbours.push_back({edge.data, edge.to});
    };

    const Rank &fromRank = getVertexParent(edge.from);
    const Rank &toRank = getVertexParent(edge.to);

    auto fromFuture = upcxx::rpc(fromRank, [this, insertSingleEdge] (Edge edge) { insertSingleEdge(edge); }, edge);
    auto toFuture = upcxx::rpc(toRank, [this, insertSingleEdge] (Edge edge) { insertSingleEdge(edge); }, Edge{edge.to, edge.from, edge.data});

    fromFuture.wait();
    toFuture.wait();

  return true;
}

template <typename VertexData, typename EdgeData>
bool Graph<VertexData, EdgeData>::HasEdge(const Id &a, const Id &b) const {
  // Loops are not allowed
  if (a == b) {
    return false;
  }

  return hasEdgeHelper(a, b);
}

template <typename VertexData, typename EdgeData>
bool Graph<VertexData, EdgeData>::hasEdgeHelper(const Id &a,
                                                const Id &b) const {
//  // Get rank of the parent of a first node
//  const Rank &aRank = getVertexParent(a);

//  auto singleEdgeFindLambda = [](graph_storage_type &graph, Id idA, Id idB) {
//    auto itIdA = graph->find(idA);
//    if (itIdA == graph->end()) {
//      return false;
//    }

//    upcxx::global_ptr<Vertex> aPtr = graph[idA]->second;
//    auto localAPtr = aPtr.local();
//    if (!aPtr && !localAPtr) {
//        auto itB = std::find_if(
//            localAPtr->neighbours.begin(), localAPtr->neighbours.end(),
//            [idB](const auto &weightNode) { return weightNode.second == idB; });
//        return itB != localAPtr->neighbours.end();
//    }
//  };

//  upcxx::future<bool> aFuture =
//      upcxx::rpc(aRank, singleEdgeFindLambda, m_vertexStore, a, b);

//  return aFuture.wait();
}

template <typename VertexData, typename EdgeData>
Rank Graph<VertexData, EdgeData>::getVertexParent(const Id &id) const {
  const auto rank_n = upcxx::rank_n();
  Id r = 0;
  for (; r < rank_n; ++r) {
    if ((id >= r * m_verticesPerRank) &&
        (id <= (r + 1) * m_verticesPerRank - 1)) {
      break;
    }
  }

  if (r == rank_n) {
    logMsg("Wrond parent for " + std::to_string(id) + " is " + std::to_string(r));
    --r;
  }

  return r;
}

template <typename VertexData, typename EdgeData>
upcxx::global_ptr<typename Graph<VertexData, EdgeData>::Vertex>
Graph<VertexData, EdgeData>::fetchVertexFromStore(const Id &id) const {
//  const Rank idParentRank = getVertexParent(id);
//  upcxx::future<upcxx::global_ptr<Vertex>> vertexFuture = upcxx::rpc(
//      idParentRank,
//      [](graph_storage_type &graph, Id id) {
//        auto idIt = graph->find(id);
//        if (idIt == graph->end()) {
//          return upcxx::global_ptr<Vertex>(nullptr);
//        }

//        return idIt->second;
//      },
//      m_vertexStore, id);

//  return vertexFuture.wait();
}
}; // namespace PGASGraph

#endif // PGAS_GRAPH_H
