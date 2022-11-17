#pragma once

#include <pgas-graph/pgas-graph.h>

namespace PGASGraph::Algorithms::Gossip {
    // TODO: Works only for connected graphs! Need to refine termination predicate. Can be research subject.
    // TODO: Each process should traverse its component.
    // TODO: How equal graph partitioning works on Knodel graphs?
    template <typename Vertex, typename Edge>
    void PushRandomizedGossip(Graph<Vertex, Edge>& graph, typename Vertex::Id vertexId, typename Vertex::Data data)
    {
        using PGASGraphType = PGASGraph::Graph<Vertex, Edge>;
        using GraphStorageType = typename PGASGraphType::GraphStorageType;
        using VertexId = typename Vertex::Id;

        //const auto uniformRandomInt = [](const int start, const int end)
        //{
        //    std::random_device rd;
        //    std::mt19937 gen(rd());
        //    std::uniform_int_distribution<> distrib(start, end);
        //    return distrib(gen);
        //};

        // std::size_t globalVertexCount{0};
        // for (Rank r{0}; r < upcxx::rank_n(); ++r) {
        //     const auto localStorage{graph.GetGraphStorage().fetch(r)};
        //     globalVertexCount += localStorage->size();
        // }

        // upcxx::barrier();

        // std::unique_ptr<int64_t[]> insertsPerRank(new int64_t[upcxx::rank_n()]());

        std::cout << "[debug]: Starting randomized push-based gossip algorithm from vertex with Id " << vertexId.ToString() << " and data " << data << std::endl;
        const auto startTime = std::chrono::high_resolution_clock::now();

        const auto localStorage{ graph.GetGraphStorage().fetch(upcxx::rank_me()).wait() };
        logMsg("localStorage=" + std::to_string(localStorage.size()));

        upcxx::barrier();

        // TODO: Effective storage to search for non-negative ids
        upcxx::dist_object<std::vector<VertexId>> visited{{}};
        auto getVisitedSize = [](upcxx::dist_object<std::vector<VertexId>>& visited) {
            return upcxx::rpc(upcxx::rank_me(), [](upcxx::dist_object<std::vector<VertexId>>& visited) {
                return visited->size();
            }, visited).wait();
        };

        auto getRandomVertexFromLocalStore = [](GraphStorageType& storage) {
            return upcxx::rpc(upcxx::rank_me(), [](GraphStorageType& storage) {
                // TODO: Use uniform prng
                return storage->operator[](0);
            }, storage).wait();
        };

        auto pushRandomizedGossip = [](GraphStorageType& storage, typename Vertex::Id vertexId, typename Vertex::Data data, upcxx::dist_object<std::vector<VertexId>>& visited)
        {
            //// TODO: VertexId may not always directly map to vertex store index, so we need to perform O(N) search here.
            // const auto localVertexStore = graph->fetch(upcxx::rank_me()).wait();
            const auto vertexStoreSize{ storage->size() };
            Vertex* vertex{ nullptr };
            for (std::size_t idx{ 0 }; idx < vertexStoreSize; ++idx) {
                auto* localVertex{ storage->operator[](idx) };
                if (localVertex) {
                    // std::cout << "localVertex=" << localVertex->ToString() << std::endl;
                    if (localVertex->id == vertexId) {
                        vertex = localVertex;
                        break;
                    }
                }
            }

            if (!vertex) {
                std::cerr << "[ERROR]: Unable to find vertex with VertexId=" << vertexId.ToString()
                    << " inside vertex store of rank=" << upcxx::rank_me() << std::endl;
                return VertexId{};
            }

            // Update current vertex data
            logMsg("[VAGAG]: visited.size()=" + std::to_string(visited->size()));
            vertex->data = data;
            visited->push_back(vertexId);
            // insertsPerRank[upcxx::rank_me()]++;

            // Choose uniformly random neighbour
            // const auto randomNeighbourId{ uniformRandomInt(0, vertex->neighbourhood.size()) };
            // const auto neighbour{ vertex->neighbourhood[randomNeighbourId] };
            // const neighbourIdParent{ getVertexParent(neighbourId) };

            for (auto neighbour : vertex->neighbourhood) {
                auto it = std::find(visited->begin(), visited->end(), neighbour.id);
                // Try to find non-visited neighbour to return, if it's not possible, then return just the last one.
                if (it == visited->end()) {
                    return neighbour.id;
                }
                else {
                    return neighbour.id;
                }
            }

            // PushPullRandomizedGossip(neighbour.id, data);
            auto result = VertexId{};
            result.isNull = true;
            return result;
        };

        typename Vertex::Id nextVertexId = getRandomVertexFromLocalStore(graph.GetGraphStorage())->id;
        while (getVisitedSize(visited) != localStorage.size()) {
            logMsg("aaa");
            const auto vertexIdParent{ graph.GetVertexParent(nextVertexId) };
            nextVertexId = upcxx::rpc(
                vertexIdParent,
                pushRandomizedGossip,
                graph.GetGraphStorage(), nextVertexId, data, visited).wait();
            upcxx::barrier();
        }

        upcxx::barrier();

        auto end = std::chrono::steady_clock::now();
        const auto stopTime = std::chrono::high_resolution_clock::now();
        std::cout << "[debug]: Finished randomized push-based gossip algorithm" << std::endl;
        std::cout << "[debug]: Gossiping took " << std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime).count() << "ms" << std::endl;
    }
}
