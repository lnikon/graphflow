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

namespace PGASGraph
{

#define DEBUG_LOG_MSG

/*! \brief Print specified message in a "[i/n]: <msg>" format.
 * \param msg message to be printed.
 */
void logMsg(const std::string& msg);
void logMsg(const std::string& msg, const size_t rank);

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
template <typename VertexData, typename EdgeData> class Graph
{
public:
    /*! \brief Represents edge in a graph.
     */
    struct Edge
    {
        // Edge has two ends.
        Id from;
        Id to;

        // User supplied data associated with each edge.
        EdgeData data;

        /*! \brief Return the inverted edge.
         */
        Edge Invert() const
        {
            return Edge{to, from, data};
        }

        bool operator<(const Edge& other)
        {
            return data < other.data;
        }

        bool operator>(const Edge& other)
        {
            return !(*this < other);
        }

        /*! \brief Serialize the edge into the string.
         */
        std::string ToString()
        {
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
     * Each vertex has an unique id.
     */
    struct Vertex
    {
        // A unique local id.
        Id id;

        // A value stored in the vertex.
        VertexData value;

        /* A list of Id's of the neighbours.
         * Id will be used to fetch actual pointer from the vertex store.
         */
        using weight_node_t = std::pair<EdgeData, Id>;
        std::vector<weight_node_t> neighbours;

        /*! \brief Default constructor.
         */
        Vertex() = default;

        /*! \brief Construct vertex by a given id.
         */
        Vertex(const Id id)
            : id(id)
        {
        }

        /*! \brief Serialize the vertex into the string.
         */
        std::string ToString() const
        {
            std::string result;
            result += "{id=" + std::to_string(id);
            result += "}";
            return result;
        }
    };

    /*! \brief Construct instance of the graph owned by current rank.
     *
     * \param @totalNumberVertices Number of vertices in the graph.
     * \param @verticesPerRank Number of the vertices in the current cut.
     */
    Graph(const size_t totalNumberVertices, const size_t verticesPerRank);

    /*! \brief Internal representations of the graph as a vertex store.
     * Currently vertex_store_type is used.
     *
     * \param vertex_store_type Map from the vertex Id to its global pointer.
     * \param graph_storage_type Type of the storage used for the graph.
     */
    using vertex_store_type = std::vector<Vertex*>;
    using graph_storage_type = vertex_store_type;

    /*! \brief Adds an undirected edge between two nodes.
     *
     *  \param a First node.
     *  \param b Second node.
     */
    bool AddEdge(const Edge& edge);

    /*! \brief Check if two nodes are connected.
     *
     *  \param a First node.
     *  \param b Second node.
     */
    bool HasEdge(const Id& a, const Id& b) const;

    /*! \brief Return minimal id that is stored on the current rank.
     */
    Id minId() const
    {
        return upcxx::rank_me() * m_verticesPerRank;
    }

    /*! \brief Return maximal id that is stored on the current rank.
     */
    Id maxId() const
    {
        return (upcxx::rank_me() + 1) * m_verticesPerRank - 1;
    }

    /*! \brief Print the cut owned by the current rank.
     */
    void printLocal();

    /*! \brief Calculate minimum spanning tree of the graph using Prims algorithm.
     */
    upcxx::dist_object<std::vector<std::pair<EdgeData, std::pair<Id, Id>>>> MST();

    /*! \brief Calculate minimum spanning tree of the graph using Kruskal
     * algorithm.
     */
    upcxx::dist_object<std::vector<typename Graph<VertexData, EdgeData>::Edge>> Kruskal();

    /*! \brief Export the whole graph into the file.
     *
     *  \param fileName name of the file to export the graph.
     */
    void ExportIntoFile(const std::string& fileName) const;

    /*! \brief Return rank of the specified vertex.
     *
     *  \param Id of the vertex.
     */
    Rank getVertexParent(const Id& a) const;

private:
    graph_storage_type m_vertexStore;
    const size_t m_totalNumberVertices;
    const size_t m_verticesPerRank;

    /*
     * Helper methods
     */
    bool addEdgeHelper(Edge edge);
    bool hasEdgeHelper(const Id& a, const Id& b) const;
    upcxx::global_ptr<Vertex> fetchVertexFromStore(const Id& id) const;

    size_t normilizeId(const Id id) const
    {
        return id - getVertexParent(id) * m_vertexStore.size();
    }
};

template <typename VertexData, typename EdgeData>
bool Graph<VertexData, EdgeData>::AddEdge(const Edge& edge)
{
    // Loops are not allowed.
    if (edge.from == edge.to)
    {
        return false;
    }

    return addEdgeHelper(edge);
}

template <typename VertexData, typename EdgeData>
Graph<VertexData, EdgeData>::Graph(const size_t totalNumberVertices, const size_t verticesPerRank)
    : m_vertexStore({})
    , m_totalNumberVertices(totalNumberVertices)
    , m_verticesPerRank(verticesPerRank)
{

    // Allocate space for the current cut.
    m_vertexStore.resize(verticesPerRank);
}

template <typename VertexData, typename EdgeData>
bool Graph<VertexData, EdgeData>::addEdgeHelper(Edge edge)
{
    // Don't allow self loops.
    if (edge.from == edge.to)
    {
        return false;
    }

    // Lambda to insert single edge into the graph.
    auto insertSingleEdge = [this](Edge edge) {
        const auto parentRank = getVertexParent(edge.from);
        const size_t idx = edge.from - parentRank * m_vertexStore.size();
        assert(idx < m_vertexStore.size());
        if (!m_vertexStore[idx])
        {
            m_vertexStore[idx] = new Vertex(edge.from);
        }

        assert(m_vertexStore[idx]);

        auto& neighbours = m_vertexStore[idx]->neighbours;
        for (auto& ngh : neighbours)
        {
            if (ngh.first == edge.data || ngh.second == edge.to)
            {
                return false;
            }
        }

        neighbours.push_back({edge.data, edge.to});
        return true;
    };

    const Rank& fromRank = getVertexParent(edge.from);
    const Rank& toRank = getVertexParent(edge.to);

    // auto fromFuture =
    upcxx::rpc(
        fromRank, [this, insertSingleEdge](Edge edge) { insertSingleEdge(edge); }, edge)
        .wait();
    // auto toFuture =
    upcxx::rpc(
        toRank, [this, insertSingleEdge](Edge edge) { insertSingleEdge(edge); }, edge.Invert())
        .wait();

    // fromFuture.wait();
    // toFuture.wait();

    return true;
}

template <typename VertexData, typename EdgeData>
bool Graph<VertexData, EdgeData>::HasEdge(const Id& a, const Id& b) const
{
    // Loops are not allowed
    if (a == b)
    {
        return false;
    }

    return hasEdgeHelper(a, b);
}

template <typename VertexData, typename EdgeData> void Graph<VertexData, EdgeData>::printLocal()
{
    const auto& localVertexStore = m_vertexStore;
    std::string msg;
    for (auto idx = size_t{0}; idx < localVertexStore.size(); ++idx)
    {
        msg += "\n" + std::to_string(localVertexStore[idx]->id) + ": [";
        auto neighs = localVertexStore[idx]->neighbours;
        for (const auto& neigh : neighs)
        {
            msg += " (" + std::to_string(neigh.first) + ", " + std::to_string(neigh.second) + "), ";
        }
        msg += "]\n";
    }

    logMsg(msg);
}

template <typename VertexData, typename EdgeData>
upcxx::dist_object<std::vector<std::pair<EdgeData, std::pair<Id, Id>>>>
Graph<VertexData, EdgeData>::MST()
{
    using added_set_t = upcxx::dist_object<std::unordered_set<Id>>;
    using weight_node_t = typename Vertex::weight_node_t;
    using local_edge_t = std::pair<EdgeData, std::pair<Id, Id>>;
    using dist_local_edge_vec_t = upcxx::dist_object<std::vector<local_edge_t>>;

    // Process with rank 0 is the master.
    const Rank masterRank = 0;

    auto ToString = [](local_edge_t edge) {
        std::string result;
        result += "{from=";
        result += std::to_string(edge.second.first);
        result += ", to=";
        result += std::to_string(edge.second.second);
        result += ", weight=";
        result += std::to_string(edge.first);
        result += "}\n";
        return result;
    };

    auto addIdIntoMST = [this](added_set_t& addedSet, Id id) {
        upcxx::rpc(
            getVertexParent(id),
            [](added_set_t& addedSet, Id id) { addedSet->insert(id); },
            addedSet,
            id)
            .wait();
    };

    auto isIdInMST = [this](added_set_t& addedSet, Id id) {
        return upcxx::rpc(
                   getVertexParent(id),
                   [](added_set_t& addedSet, Id id) {
                       return addedSet->find(id) != addedSet->end();
                   },
                   addedSet,
                   id)
            .wait();
    };

    auto addedSetSizeOnRank = [](added_set_t& addedSet, int rank) {
        return upcxx::rpc(
                   rank, [](added_set_t& addedSet) { return addedSet->size(); }, addedSet)
            .wait();
    };

    auto addedSetSize = [addedSetSizeOnRank](added_set_t& addedSet) {
        return addedSetSizeOnRank(addedSet, upcxx::rank_me());
    };

    auto globalAddedSetSize = [addedSetSizeOnRank](added_set_t& addedSet) {
        size_t count = 0;
        for (auto r = 0; r < upcxx::rank_n(); ++r)
        {
            count += addedSetSizeOnRank(addedSet, r);
        }
    };

    auto addEdgeIntoMst = [](dist_local_edge_vec_t& mstEdges, local_edge_t edge) {
        upcxx::rpc(
            0,
            [](dist_local_edge_vec_t& mstEdges, local_edge_t edge) {
                if (std::find_if(mstEdges->begin(), mstEdges->end(), [edge](const auto& current) {
                        return edge.first == current.first &&               // weight
                               edge.second.first == current.second.first && // from
                               edge.second.second == current.second.second; // to
                    }) == mstEdges->end())
                {
                    auto ToString = [](local_edge_t edge) {
                        std::string result;
                        result += "{from=";
                        result += std::to_string(edge.second.first);
                        result += ", to=";
                        result += std::to_string(edge.second.second);
                        result += ", weight=";
                        result += std::to_string(edge.first);
                        result += "}\n";
                        return result;
                    };
                    // logMsg("mstSize: " + std::to_string(mstEdges->size()) +
                    // ToString(edge));
                    mstEdges->push_back(edge);
                }
            },
            mstEdges,
            edge)
            .wait();
    };

    dist_local_edge_vec_t localMst{{}};
    auto& localVertexStore = m_vertexStore;
    if (localVertexStore.empty())
    {
        logMsg("Empty local vertex store.");
        return localMst;
    }

    // Each worker has its portion.
    added_set_t addedSet{{}};

    // Add initial random vertex into the MST.
    bool isEmptyLocalVertexStore{false};
    {
        isEmptyLocalVertexStore = localVertexStore.empty();
        if (isEmptyLocalVertexStore)
        {
            return localMst;
        }

        const Id initialVertex = (*localVertexStore.begin())->id;

        addIdIntoMST(addedSet, initialVertex);
        Vertex* initialVertexPtr = localVertexStore[normilizeId(initialVertex)];
        local_edge_t minEdge =
            std::make_pair(std::numeric_limits<int>::max(), std::make_pair(0, 0));
        for (const auto& neigh : initialVertexPtr->neighbours)
        {
            if (neigh.first < minEdge.first)
            {
                minEdge.first = neigh.first;
                minEdge.second.first = initialVertexPtr->id;
                minEdge.second.second = neigh.second;
            }
        }

        // logMsg("Initial vertex: " + ToString(minEdge));

        addEdgeIntoMst(localMst, minEdge);
    }

    auto isMSTFull = [this](dist_local_edge_vec_t& localMst) -> bool {
        return upcxx::rpc(
                   0,
                   [](dist_local_edge_vec_t& localMst, size_t totalVertexCount) {
                       if (localMst->empty())
                       {
                           return false;
                       }
                       //  logMsg(
                       //      "localMst->size(): " + std::to_string(localMst->size())
                       //      +
                       //      ", totalVertexCount: " +
                       //      std::to_string(totalVertexCount));
                       return localMst->size() == totalVertexCount - 1;
                   },
                   localMst,
                   m_totalNumberVertices)
            .wait();
    };

    auto getMSTSize = [this](dist_local_edge_vec_t& localMst) -> bool {
        return upcxx::rpc(
                   0, [](dist_local_edge_vec_t& localMst) { return localMst->size(); }, localMst)
            .wait();
    };

    upcxx::barrier();

    while (!isMSTFull(localMst) && !isEmptyLocalVertexStore)
    {
        // Min edge in the current cut.
        local_edge_t minEdge =
            std::make_pair(std::numeric_limits<int>::max(), std::make_pair(0, 0));

        // Find edge with minimum weight that connects vertex in the current cut to
        // the MST.
        for (const auto& globalVertex : localVertexStore)
        {
            Vertex* localVertex = globalVertex;
            const bool isCurrentInMst = isIdInMST(addedSet, localVertex->id);
            for (const auto& neigh : localVertex->neighbours)
            {
                if (neigh.first < minEdge.first)
                {
                    if (isCurrentInMst && !isIdInMST(addedSet, neigh.second))
                    {
                        minEdge.first = neigh.first;
                        minEdge.second.first = localVertex->id;
                        minEdge.second.second = neigh.second;
                    }
                }
            }
        }

        upcxx::barrier();

        local_edge_t globalMinEdge = upcxx::reduce_one(
                                         minEdge,
                                         [](const local_edge_t& lhs, const local_edge_t& rhs) {
                                             return (lhs.first < rhs.first ? lhs : rhs);
                                         },
                                         0)
                                         .wait();

        if (globalMinEdge.first == std::numeric_limits<int>::max())
        {
            break;
        }

        upcxx::barrier();

        if (0 == upcxx::rank_me())
        {
            if (globalMinEdge.first != std::numeric_limits<int>::max())
            {
                addIdIntoMST(addedSet, globalMinEdge.second.second);
                addEdgeIntoMst(localMst, globalMinEdge);
            }
        }
    }

    if (0 == upcxx::rank_me())
    {
        upcxx::rpc(
            0,
            [ToString](dist_local_edge_vec_t& localMst) {
                size_t cost{0};
                auto begin = localMst->begin();
                for (; begin != localMst->end(); ++begin)
                {
                    cost += begin->first;
                }

                logMsg("MST Cost: " + std::to_string(cost));
            },
            localMst)
            .wait();
    }

    upcxx::barrier();
    return localMst;
}

template <typename VertexData, typename EdgeData>
upcxx::dist_object<std::vector<typename Graph<VertexData, EdgeData>::Edge>>
Graph<VertexData, EdgeData>::Kruskal()
{
    struct UnionFind
    {
        explicit UnionFind(const size_t n)
        {
            for (size_t i = 0; i < n; ++i)
            {
                makeSet(i);
            }
        }
        void makeSet(Id v)
        {
            m_parent[v] = v;
        }

        Id findSet(Id v)
        {
            if (m_parent.find(v) == m_parent.end())
            {
                m_parent[v] = v;
                return v;
            }

            if (m_parent[v] == v)
            {
                return v;
            }

            return findSet(m_parent[v]);
        }

        void unionSets(Id a, Id b)
        {
            a = findSet(a);
            b = findSet(b);
            if (a != b)
            {
                m_parent[b] = a;
            }
        }

    private:
        std::unordered_map<Id, Id> m_parent;
        std::unordered_map<Id, size_t> m_rank;
    };

    using edges_t = std::vector<Edge>;
    auto unionEdges = [](edges_t& lhs, edges_t& rhs) {
        edges_t result;

        std::sort(lhs.begin(), lhs.end());
        std::sort(rhs.begin(), rhs.end());

        std::set_union(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::back_inserter(result));

        return result;
    };

    using dist_mst_edges_t = upcxx::dist_object<edges_t>;
    auto addEdgeIntoMST = [](dist_mst_edges_t& mst, Rank rank, Edge edge) {
        assert(rank >= 0 && rank < upcxx::rank_n());
        upcxx::rpc(
            rank, [](dist_mst_edges_t& mst, Edge edge) { mst->push_back(edge); }, mst, edge)
            .wait();
    };

    auto addEdgesIntoMST = [](dist_mst_edges_t& mst, Rank rank, edges_t edges) {
        assert(rank >= 0 && rank < upcxx::rank_n());
        upcxx::rpc(
            rank,
            [](dist_mst_edges_t& mst, edges_t edges) {
                for (auto& edge : edges)
                {
                    mst->push_back(edge);
                }
            },
            mst,
            edges)
            .wait();
    };

    auto getMSTEdgesCount = [](dist_mst_edges_t& mst, Rank rank) {
        assert(rank >= 0 && rank < upcxx::rank_n());
        return upcxx::rpc(
                   rank, [](dist_mst_edges_t& mst) { return mst->size(); }, mst)
            .wait();
    };

    auto clearMSTEdgesAsync = [](dist_mst_edges_t& mst, Rank rank) {
        assert(rank >= 0 && rank < upcxx::rank_n());
        return upcxx::rpc(
            rank, [](dist_mst_edges_t& mst) { mst->clear(); }, mst);
    };

    auto clearMSTEdges = [clearMSTEdgesAsync](dist_mst_edges_t& mst, Rank rank) {
        assert(rank >= 0 && rank < upcxx::rank_n());
        clearMSTEdgesAsync(mst, rank).wait();
    };

    auto kruskal = [](dist_mst_edges_t& mst, Rank rank, edges_t edges, const size_t verticesCount) {
        upcxx::rpc(
            rank,
            [](dist_mst_edges_t& mst, edges_t edges, const size_t verticesCount) {
                UnionFind unionFind(verticesCount);
                for (auto edge : edges)
                {
                    if (unionFind.findSet(edge.from) != unionFind.findSet(edge.to))
                    {
                        // addEdgeIntoMST(distMSTEdges, upcxx::rank_me(), edge);
                        mst->push_back(edge);
                        unionFind.unionSets(edge.from, edge.to);
                    }
                }
            },
            mst,
            edges,
            verticesCount)
            .wait();
    };

    // Get the edge list and sort it
    auto edgesLocal = std::vector<Edge>{};
    {
        std::set<EdgeData> weights;
        for (auto* vertex : m_vertexStore)
        {
            for (auto& ngh : vertex->neighbours)
            {
                if (!weights.count(ngh.first))
                {
                    weights.insert(ngh.first);
                    edgesLocal.push_back(Edge{vertex->id, ngh.second, ngh.first});
                }
            }
        }
    }

    // Find the local portion of the MST.
    dist_mst_edges_t distMSTEdges{{}};
    kruskal(distMSTEdges, upcxx::rank_me(), edgesLocal, m_verticesPerRank);

    // Wait until each rank finds its portion of the MST.
    upcxx::barrier();

    const size_t superstepCount = std::log2(upcxx::rank_n());
    Rank rankOffset{1};
    for (size_t superstep = 1; superstep <= superstepCount; ++superstep)
    {
        const size_t divisibleBy = std::pow(2, superstep);
        const size_t color = upcxx::rank_me() % divisibleBy;
        const bool isMergeable = (color == 0);
        // Merge portions of the global MST found by each rank.
        if (isMergeable)
        {
            // Which ranks edges to fetch.
            const auto otherRank{upcxx::rank_me() + rankOffset};

            // Fetch the edges.
            auto globalMSTOtherFuture = distMSTEdges.fetch(otherRank);
            auto globalMSTCurrentFuture = distMSTEdges.fetch(upcxx::rank_me());

            // Wait for the edges to be on the current rank.
            auto localMSTOther = globalMSTOtherFuture.wait();
            auto localMSTCurrent = globalMSTCurrentFuture.wait();

            // Clear distributed edges while mergines MSTs.
            auto clearMSTEdgesFuture = clearMSTEdgesAsync(distMSTEdges, upcxx::rank_me());

            // Merge MST from different ranks.
            auto mergedMSTEdges{unionEdges(localMSTCurrent, localMSTOther)};

            // Wait for the edge clearing to be finished.
            clearMSTEdgesFuture.wait();

            // Clear the other rank MST edges.

            // Add merges edges into the MST.
            kruskal(distMSTEdges, upcxx::rank_me(), mergedMSTEdges, m_totalNumberVertices);

            rankOffset *= 2;
        }
        upcxx::barrier();
    }

    upcxx::barrier();
    return distMSTEdges;
}

template <typename VertexData, typename EdgeData>
void Graph<VertexData, EdgeData>::ExportIntoFile(const std::string& fileName) const
{
    if (0 == upcxx::rank_me())
    {
        {
            std::fstream stream(fileName,
                                std::fstream::in | std::fstream::out | std::fstream::trunc);
        }

        const auto& rank_n = upcxx::rank_n();
        for (size_t r = 0; r < rank_n; ++r)
        {
            upcxx::rpc(
                r,
                [this](std::string fileName) {
                    std::fstream stream(fileName,
                                        std::fstream::in | std::fstream::out | std::fstream::app);
                    if (!stream.is_open())
                    {
                        logMsg("Unable to open " + fileName + " to write into\n");
                        return;
                    }

                    int i = 0;
                    for (Vertex* vertex : m_vertexStore)
                    {
                        assert(vertex != nullptr);
                        for (auto& ngh : vertex->neighbours)
                        {
                            stream << vertex->id << " " << ngh.second << "\n";
                        }
                        ++i;
                    }
                },
                fileName)
                .wait();
        }
    }

    upcxx::barrier();
}

template <typename VertexData, typename EdgeData>
bool Graph<VertexData, EdgeData>::hasEdgeHelper(const Id& a, const Id& b) const
{
    //  // Get rank of the parent of a first node
    //  const Rank &aRank = getVertexParent(a);

    //  auto singleEdgeFindLambda = [](graph_storage_type &graph, Id idA, Id idB)
    //  {
    //    auto itIdA = graph->find(idA);
    //    if (itIdA == graph->end()) {
    //      return false;
    //    }

    //    upcxx::global_ptr<Vertex> aPtr = graph[idA]->second;
    //    auto localAPtr = aPtr.local();
    //    if (!aPtr && !localAPtr) {
    //        auto itB = std::find_if(
    //            localAPtr->neighbours.begin(), localAPtr->neighbours.end(),
    //            [idB](const auto &weightNode) { return weightNode.second == idB;
    //            });
    //        return itB != localAPtr->neighbours.end();
    //    }
    //  };

    //  upcxx::future<bool> aFuture =
    //      upcxx::rpc(aRank, singleEdgeFindLambda, m_vertexStore, a, b);

    //  return aFuture.wait();
}

template <typename VertexData, typename EdgeData>
Rank Graph<VertexData, EdgeData>::getVertexParent(const Id& id) const
{
    const auto rank_n = upcxx::rank_n();
    Id r = 0;
    for (; r < rank_n; ++r)
    {
        if ((id >= r * m_verticesPerRank) && (id <= (r + 1) * m_verticesPerRank - 1))
        {
            break;
        }
    }

    if (r == rank_n)
    {
        logMsg("Wrong parent for " + std::to_string(id) + " is " + std::to_string(r));
        --r;
    }

    return r;
}

template <typename VertexData, typename EdgeData>
upcxx::global_ptr<typename Graph<VertexData, EdgeData>::Vertex>
Graph<VertexData, EdgeData>::fetchVertexFromStore(const Id& id) const
{
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
