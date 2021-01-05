#ifndef PGAS_GRAPH_H
#define PGAS_GRAPH_H

#include <upcxx/upcxx.hpp>

#include <type_traits>
#include <unordered_map>

namespace PGASGraph {

/*!
 * Type for the vertex Id
 */
using Id = unsigned long long int;

/*!
 * Type for the vertex parent rank
 */
using Rank = upcxx::intrank_t;

/*! \brief An adjacency list distributed graph.
 *
 * Each vertex has assigned an Id which is unique across all the
 * vertices. That Id is particularly used for graph distribution by modulo rank
 * size e.g. vertex->id % upcxx::rank_n().
 *
 */
template <typename ValueType> class Graph {
public:
  /*! \brief Represents vertex in a graph.
   *
   * Each vertex has an unique id.
   */
  struct Vertex {
    /// A unique id.
    Id id;

    /// A value stored in that vertex.
    ValueType value;

    /// A list of Id's of the neighbours.
    /// Id will be used to fetch actual pointer from the vertex store.
    std::vector<Id> neighbours;

    Vertex(Id id) : id(id) {}
  };

  Graph();

  /*
   * Internal representations of the graph as a vertex store.
   * Currently vertex_store_type is used.
   *
   * - 'vertex_store_type' Map from the vertex Id to its global pointer.
   * - 'graph_storage_type' Type of the storage used for the graph.
   */
  using vertex_store_type =
      upcxx::dist_object<std::unordered_map<Id, upcxx::global_ptr<Vertex>>>;
  using graph_storage_type = vertex_store_type;

  /*! \brief Adds an undirected edge between two nodes
   *
   *  \param a First node
   *  \param b Second node
   */
  bool AddEdge(const Id &a, const Id &b);

  /*! \brief Check if two nodes are connected.
   *
   *  \param a First node
   *  \param b Second node
   */
  bool HasEdge(const Id &a, const Id &b);

private:
  graph_storage_type m_vertexStore;

  /*
   * Helper methods
   */
  bool addEdgeHelper(const Id &a, const Id &b);
  bool hasEdgeHelper(const Id &a, const Id &b);

  Rank getVertexParent(const Id &a) const;
  upcxx::global_ptr<Vertex> fetchVertexFromStore(const Id &id) const;
};

template <typename ValueType>
bool Graph<ValueType>::AddEdge(const Id &a, const Id &b) {
  // Loops are not allowed
  if (a == b) {
    return false;
  }

  return addEdgeHelper(a, b);
}

template <typename ValueType> Graph<ValueType>::Graph() : m_vertexStore({}) {}

template <typename ValueType>
bool Graph<ValueType>::addEdgeHelper(const Id &a, const Id &b) {
  // Get rank of the first vertex parent
  const Rank &aRank = getVertexParent(a);

  // Get rank of the second vertex parent
  const Rank &bRank = getVertexParent(b);

  auto singleEdgeInsertLambda = [](graph_storage_type &graph, Id idA, Id idB) {
    auto itId = graph->find(idA);
    if (itId == graph->end()) {
      graph->insert({idA, upcxx::new_<Vertex>(idA)});
    }

    itId = graph->find(idA);
    upcxx::global_ptr<Vertex> aPtr = itId->second;
    auto localAPtr = aPtr.local();
    localAPtr->neighbours.push_back(idB);
  };

  upcxx::future<> aFuture =
      upcxx::rpc(aRank, singleEdgeInsertLambda, m_vertexStore, a, b);

  upcxx::future<> bFuture =
      upcxx::rpc(bRank, singleEdgeInsertLambda, m_vertexStore, b, a);

  aFuture.wait();
  bFuture.wait();

  return true;
}

template <typename ValueType>
bool Graph<ValueType>::HasEdge(const Id &a, const Id &b) {
  // Loops are not allowed
  if (a == b) {
    return false;
  }

  return hasEdgeHelper(a, b);
}

template <typename ValueType>
bool Graph<ValueType>::hasEdgeHelper(const Id &a, const Id &b) {
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
	auto itB = std::find(localAPtr->neighbours.begin(), localAPtr->neighbours.end(), idB);
	return itB != localAPtr->neighbours.end();
  };

  upcxx::future<bool> aFuture =
      upcxx::rpc(aRank, singleEdgeInsertLambda, m_vertexStore, a, b);

  return aFuture.wait();
}

template <typename ValueType>
Rank Graph<ValueType>::getVertexParent(const Id &id) const {
  const auto size{upcxx::rank_n()};
  return id % size;
}

template <typename ValueType>
upcxx::global_ptr<typename Graph<ValueType>::Vertex>
Graph<ValueType>::fetchVertexFromStore(const Id &id) const {
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
