#ifndef PGAS_GRAPH_H
#define PGAS_GRAPH_H

#include <upcxx/upcxx.hpp>

#include <queue>
#include <type_traits>
#include <unordered_map>

namespace PGASGraph {

void logMsg(const std::string &msg) {
  std::cout << "[" << upcxx::rank_me() << "/" << upcxx::rank_n() << "] " << msg
            << "\n";
}

/*!
 * Type for the vertex Id.
 */
using Id = unsigned long long int;

/*!
 * Type for the vertex parent rank.
 */
using Rank = upcxx::intrank_t;

/*! \brief An adjacency list distributed graph.
 *
 * Each vertex has assigned an Id which is unique across all the
 * vertices. That Id is particularly used for graph distribution by modulo rank
 * size e.g. vertex->id % upcxx::rank_n().
 *
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
  };

  /*! \brief Represents vertex in a graph.
   *
   * Each vertex has an unique id.
   */
  struct Vertex {
    /// A unique id.
    Id id;

    /// A value stored in that vertex.
    VertexData value;

    /// A list of Id's of the neighbours.
    /// Id will be used to fetch actual pointer from the vertex store.
    std::vector<std::pair<Id, EdgeData>> neighbours;

    Vertex(Id id) : id(id) {}
  };

  Graph();

  /*
   * Internal representations of the graph as a vertex store.
   * Currently vertex_store_type is used.
   *
   * @param vertex_store_type Map from the vertex Id to its global pointer.
   * @param graph_storage_type Type of the storage used for the graph.
   */
  using vertex_store_type =
      upcxx::dist_object<std::unordered_map<Id, upcxx::global_ptr<Vertex>>>;
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

  bool MST() {
    using added_set_t = upcxx::dist_object<std::unordered_set<Id>>;
    using mst_edge_t = std::pair<std::pair<Id, Id>, EdgeData>;
    using mst_edge_collection_t = upcxx::dist_object<std::vector<mst_edge_t>>;

    // Process with rank 0 is the master.
    const Rank masterRank = 0;

    // This set is used to track Id's of those nodes, that are already in the
    // MST
    added_set_t addedSet{{}};
    mst_edge_collection_t mstEdges{{}};

    // This lambda used to add node into MST
    auto addNodeIntoMST = [this](added_set_t &addedSet, Id id) {
      upcxx::rpc(
          getVertexParent(id), [](added_set_t &set, Id id) { set->insert(id); },
          addedSet, id)
          .wait();
    };

    // This lambda is used to check if node is present in MST
    auto isInMST = [this](added_set_t &addedSet, Id id) {
      bool addedIntoMST = upcxx::rpc(
                              getVertexParent(id),
                              [](added_set_t &addedSet, Id id) {
                                return addedSet->find(id) != addedSet->end();
                              },
                              addedSet, id)
                              .wait();
      return addedIntoMST;
    };

    // Should be true, when all nodes in the current vertex store are connected
    // to MST ???
    bool done = false;
    using global_minimums_t =
        upcxx::dist_object<std::vector<std::pair<std::pair<Id, Id>, EdgeData>>>;
    struct local_edge {
      Id from;
      Id to;
      EdgeData weight;
    };
    using global_edge_collection_t =
        upcxx::dist_object<std::vector<local_edge>>;

    global_edge_collection_t edgeCollection{{}};
    global_minimums_t globalMinimums{{}};
    if (masterRank == upcxx::rank_me()) {
      upcxx::rpc(
          masterRank,
          [](global_minimums_t &globalMinimums) {
            globalMinimums->resize(upcxx::rank_n());
          },
          globalMinimums)
          .wait();
    }

    using rank_state_t = upcxx::dist_object<std::set<Rank>>;
    rank_state_t rankState{{}};

    bool oneVertex = false;
    while (!done) {
      // Only worker nodes should search min weight edge.
      bool found = false;
      EdgeData minWeight{std::numeric_limits<int>::min()};
      Id minId{};
      Id fromId{};
      if (masterRank != upcxx::rank_me()) {
        for (const auto &[id, globalVertex] :
             m_vertexStore.fetch(upcxx::rank_me()).wait()) {
          assert(globalVertex.is_local());
          Vertex *localVertex = globalVertex.local();

          // Find edge with min weight that is not in MST.
          logMsg("Find edge with min weight that is not in MST");
          minWeight = localVertex->neighbours.begin()->second;
          fromId = localVertex->id;
          minId = localVertex->neighbours.begin()->first;
          if (localVertex->neighbours.size() == 1) {
            logMsg("Found only one vertex");
            found = true;
            oneVertex = true;
          } else {
            for (const auto &[id, edgeData] : localVertex->neighbours) {
              if (minWeight > edgeData) {
                if (!isInMST(addedSet, id)) {
                  minWeight = edgeData;
                  fromId = localVertex->id;
                  minId = id;
                  found = true;
                }
              }
            }
          }
        }

        if (found && oneVertex) {
          if (isInMST(addedSet, minId)) {
            logMsg("done\n");
            done = true;
          }
        }

        if (found && !done) {
          // If you are worker node, then send to the master the minimal edge
          // that you've found.
          logMsg("Adding vertex into MST");
          upcxx::rpc(
              masterRank,
              [](global_minimums_t &globalMinimums, Rank senderRank, Id fromId,
                 Id id, EdgeData weight) {
                // std::cout << "senderRank=" << senderRank << "\n";
                globalMinimums->at(senderRank) =
                    std::make_pair(std::make_pair(fromId, id), weight);
              },
              globalMinimums, upcxx::rank_me(), fromId, minId, minWeight)
              .wait();
        } else {
          // std::cout << "rank=" << upcxx::rank_me() << "\n";
          done = true;
        }
      }

      // Wait until all the workers updated vector of minimal edges with
      // partial findings.
      upcxx::barrier();
      if (done && masterRank != upcxx::rank_me()) {
        upcxx::rpc(
            upcxx::rank_me(),
            [](rank_state_t &rankState) {
              rankState->insert(upcxx::rank_me());
            },
            rankState)
            .wait();
        std::cout << "rank=" << upcxx::rank_me() << "\n";
        break;
      }

      if (masterRank == upcxx::rank_me()) {
        // If you are master, then collect edges from the workers and
        // broadcast the edge with the minimal weight.
        int minIdx = 0;
        bool atLeastOneFound = false;
        auto localMinimalEdges = globalMinimums.fetch(masterRank).wait();
        for (size_t idx = 0; idx < localMinimalEdges.size(); idx++) {
          // std::cout << "idx=" << localMinimalEdges[idx].second << "\n";
          // std::cout << "minIdx=" << localMinimalEdges[minIdx].second << "\n";
          if (localMinimalEdges[idx].second <
              localMinimalEdges[minIdx].second) {
            minIdx = idx;
            atLeastOneFound = true;
          }
        }

        // if (!atLeastOneFound) {
        //   break;
        // }

        // Add minimum edge into the MST
        for (Rank rank = 0; rank < upcxx::rank_n(); ++rank) {
          upcxx::rpc(
              rank, [](added_set_t &addedSet, Id id) { addedSet->insert(id); },
              addedSet, localMinimalEdges[minIdx].first.second)
              .wait();
        }

        upcxx::rpc(
            masterRank,
            [](mst_edge_collection_t &mstEdgeCollection, mst_edge_t edge) {
              mstEdgeCollection->push_back(edge);
            },
            mstEdges,
            std::make_pair(
                std::make_pair(localMinimalEdges[minIdx].first.first,
                               localMinimalEdges[minIdx].first.second),
                localMinimalEdges[minIdx].second))
            .wait();

        // Check if all workers are done and exit
        size_t ranksDone{0};
        for (Rank r = 0; r < upcxx::rank_n(); r++) {
          if (masterRank == r) {
            ranksDone += 1;
            continue;
          }

          ranksDone += upcxx::rpc(
                           r,
                           [](rank_state_t &rankState, Rank r) {
                             if (rankState->count(r) == 1) {
                               return 1;
                             }

                             return 0;
                           },
                           rankState, r)
                           .wait();
        }

        // std::cout << "ranksDone=" << ranksDone << "\n";
        if (ranksDone == upcxx::rank_n()) {
          std::cout << "All ranks are done\n";
          break;
        }
      }

      upcxx::barrier();
    }

    if (masterRank == upcxx::rank_me()) {
      auto localMstEdges = mstEdges.fetch(masterRank).wait();
      for (const auto &edge : localMstEdges) {
        std::cout << "from=" << edge.first.first << ", to=" << edge.first.second
                  << ", weight=" << edge.second << "\n";
      }
    }
  }

private:
  graph_storage_type m_vertexStore;

  /*
   * Helper methods
   */
  bool addEdgeHelper(Edge edge);
  bool hasEdgeHelper(const Id &a, const Id &b) const;

  Rank getVertexParent(const Id &a) const;
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
Graph<VertexData, EdgeData>::Graph() : m_vertexStore({}) {}

template <typename VertexData, typename EdgeData>
bool Graph<VertexData, EdgeData>::addEdgeHelper(Edge edge) {
  // Get rank of the first vertex parent
  const Rank &aRank = getVertexParent(edge.from);

  // Get rank of the second vertex parent
  const Rank &bRank = getVertexParent(edge.to);

  auto singleEdgeInsertLambda = [](graph_storage_type &graph, Edge edge) {
    auto itFrom = graph->find(edge.from);
    if (itFrom == graph->end()) {
      graph->insert({edge.from, upcxx::new_<Vertex>(edge.from)});
    }

    itFrom = graph->find(edge.from);
    upcxx::global_ptr<Vertex> pGlobalFrom = itFrom->second;
    auto pLocalFrom = pGlobalFrom.local();
    pLocalFrom->neighbours.push_back({edge.to, edge.data});
  };

  upcxx::future<> aFuture =
      upcxx::rpc(aRank, singleEdgeInsertLambda, m_vertexStore, edge);

  upcxx::future<> bFuture =
      upcxx::rpc(bRank, singleEdgeInsertLambda, m_vertexStore, edge);

  aFuture.wait();
  bFuture.wait();

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
  // Get rank of the parent of a first node
  const Rank &aRank = getVertexParent(a);

  // Get rank of the parent of a second node
  const Rank &bRank = getVertexParent(b);

  auto singleEdgeInsertLambda = [](graph_storage_type &graph, Id idA, Id idB) {
    auto itId = graph->find(idA);
    if (itId == graph->end()) {
      graph->insert({idA, upcxx::new_<Vertex>(idA)});
    }

    itId = graph->find(idA);
    upcxx::global_ptr<Vertex> aPtr = itId->second;
    auto localAPtr = aPtr.local();
    auto itB = std::find(localAPtr->neighbours.begin(),
                         localAPtr->neighbours.end(), idB);
    return itB != localAPtr->neighbours.end();
  };

  upcxx::future<bool> aFuture =
      upcxx::rpc(aRank, singleEdgeInsertLambda, m_vertexStore, a, b);

  return aFuture.wait();
}

template <typename VertexData, typename EdgeData>
Rank Graph<VertexData, EdgeData>::getVertexParent(const Id &id) const {
  const auto size{upcxx::rank_n()};
  return id % size;
}

template <typename VertexData, typename EdgeData>
upcxx::global_ptr<typename Graph<VertexData, EdgeData>::Vertex>
Graph<VertexData, EdgeData>::fetchVertexFromStore(const Id &id) const {
  const Rank idParentRank = getVertexParent(id);
  upcxx::future<upcxx::global_ptr<Vertex>> vertexFuture = upc(
      idParentRank,
      [](graph_storage_type &graph, Id id) {
        auto idIt = graph->find(id);
        if (idIt == graph->end()) {
          return nullptr;
        }

        return idIt->second;
      },
      m_vertexStore, id);

  return vertexFuture.wait().value();
}

}; // namespace PGASGraph

#endif // PGAS_GRAPH_H
