#pragma once

#include <pgas-graph/pgas-graph.h>

#include <sstream>
#include <exception>
#include <random>

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

        {
            std::stringstream ss;
            ss << "[debug]: Starting randomized push-based gossip algorithm from vertex with Id " << vertexId.ToString() << " and data " << data << std::endl;
            logMsg(ss.str());
        }

        const auto startTime = std::chrono::high_resolution_clock::now();

        upcxx::barrier();

        const auto localStorage{ graph.GetGraphStorage().fetch(upcxx::rank_me()).wait() };

        // TODO: Effective storage to search for non-negative ids
        upcxx::dist_object<std::vector<VertexId>> visited{ {} };
        auto getVisitedSize = [](upcxx::dist_object<std::vector<VertexId>>& visited) {
            return upcxx::rpc(upcxx::rank_me(), [](upcxx::dist_object<std::vector<VertexId>>& visited) {
                return visited->size();
                }, visited).wait();
        };

        auto getRandomVertexFromLocalStore = [](GraphStorageType& storage) {
            return upcxx::rpc(upcxx::rank_me(), [](GraphStorageType& storage) {
                logMsg("(PushRandomizedGossip::getRandomVertexFromLocalStore): storage->size()=" + std::to_string(storage->size()));
                std::default_random_engine generator;
                std::uniform_int_distribution<int> distribution(0, storage->size()-1);
                int idx = distribution(generator);
                const auto size{ storage->size() };
                std::size_t notNullCount{ 0 };
                for (std::size_t idx{ 0 }; idx < size; ++idx) {
                    if (storage->operator[](idx)) {
                        notNullCount++;
                    }
                }
                logMsg("(PushRandomizedGossip::getRandomVertexFromLocalStore): notNullCount=" + std::to_string(notNullCount));
                return storage->operator[](idx);
                }, storage).wait();
        };

        auto pushRandomizedGossip = [](GraphStorageType& storage, typename Vertex::Id vertexId, typename Vertex::Data data, upcxx::dist_object<std::vector<VertexId>>& visited)
        {
            // TODO: VertexId may not always directly map to vertex store index, so we need to perform O(N) search here.
            const auto vertexStoreSize{ storage->size() };
            Vertex* vertex{ nullptr };
            for (std::size_t idx{ 0 }; idx < vertexStoreSize; ++idx) {
                auto* localVertex{ storage->operator[](idx) };
                if (localVertex) {
                    if (localVertex->id == vertexId) {
                        vertex = localVertex;
                        break;
                    }
                }
            }

            if (!vertex) {
                {
                    std::stringstream ss;
                    ss << "(pushRandomizedGossip): [ERROR]: Unable to find vertex with VertexId=" << vertexId.ToString()
                        << " inside vertex store of rank=" << upcxx::rank_me() << std::endl;
                    logMsg(ss.str());
                }
                return VertexId{};
            }

            // Update current vertex data
            vertex->data = data;
            if (std::find(visited->begin(), visited->end(), vertexId) == visited->end()) {
                visited->push_back(vertexId);
            }

            std::random_device generator;
            std::uniform_int_distribution<int> distribution(0, vertex->neighbourhood.size()-1);
            int idx = distribution(generator);
            //std::cout << "vertex=" << vertex->ToString() << ", vertex->neighbourhood.size()=" << vertex->neighbourhood.size() << "idx=" << idx << std::endl;
            return (vertex->neighbourhood.begin() + idx)->id;

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
            return result;
        };

        upcxx::barrier();
        const auto randomVertex{ getRandomVertexFromLocalStore(graph.GetGraphStorage()) };
        if (!randomVertex) {
            throw std::logic_error("cannot get random vertex from store");
        }
        upcxx::barrier();

        typename Vertex::Id nextVertexId = randomVertex->id;
        while (getVisitedSize(visited) != localStorage.size()-1) {
            const auto vertexIdParent{ PGASGraph::GetVertexParent<Vertex>(nextVertexId, graph.GetVerticesPerRank()) };
            nextVertexId = upcxx::rpc(
                vertexIdParent,
                pushRandomizedGossip,
                graph.GetGraphStorage(), nextVertexId, data, visited).wait();

            {
                std::stringstream ss;
                ss << "getVisitedSize(visited)=" << getVisitedSize(visited) << ", localStorage.size()=" << localStorage.size() << std::endl;
                logMsg(ss.str());
            }
        }

        {
            const auto stopTime = std::chrono::high_resolution_clock::now();
            std::stringstream ss;
            ss << "[debug]: Finished randomized push-based gossip algorithm" << ". Gossiping took " << std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count() << "ms" << std::endl;
            logMsg(ss.str());
        }

        upcxx::barrier();
    }

    template <typename Vertex, typename Edge>
    void BroadcastGossip(Graph<Vertex, Edge>& graph, typename Vertex::Id vertexId, typename Vertex::Data data)
    {
        using PGASGraphType = PGASGraph::Graph<Vertex, Edge>;
        using GraphStorageType = typename PGASGraphType::GraphStorageType;
        using VertexId = typename Vertex::Id;

        {
            std::stringstream ss;
            ss << "[debug]: Starting broadcast-based gossip algorithm from vertex with Id " << vertexId.ToString() << " and data " << data << std::endl;
            logMsg(ss.str());
        }

        const auto startTime = std::chrono::high_resolution_clock::now();

        upcxx::barrier();

        const auto localStorage{ graph.GetGraphStorage().fetch(upcxx::rank_me()).wait() };

        // TODO: Effective storage to search for non-negative ids
        upcxx::dist_object<std::vector<VertexId>> visited{ {} };
        auto getVisitedSize = [](upcxx::dist_object<std::vector<VertexId>>& visited) {
            return upcxx::rpc(upcxx::rank_me(), [](upcxx::dist_object<std::vector<VertexId>>& visited) {
                return visited->size();
                }, visited).wait();
        };

        auto getRandomVertexFromLocalStore = [](GraphStorageType& storage) {
            return upcxx::rpc(upcxx::rank_me(), [](GraphStorageType& storage) {
                // TODO: Use uniform prng
                const auto size{ storage->size() };
                std::size_t notNullCount{ 0 };
                for (std::size_t idx{ 0 }; idx < size; ++idx) {
                    if (storage->operator[](idx)) {
                        notNullCount++;
                    }
                }

                return storage->operator[](0);
                }, storage).wait();
        };

        auto transferGossip = [localStorage](GraphStorageType& storage, typename Vertex::Id vertexId, typename Vertex::Data data) {
            return upcxx::rpc(PGASGraph::GetVertexParent<Vertex>(vertexId, localStorage.size()),
                [](GraphStorageType& storage, typename Vertex::Id vertexId, typename Vertex::Data data) {
                    const auto vertexStoreSize{ storage->size() };
                    Vertex* vertex{ nullptr };
                    for (std::size_t idx{ 0 }; idx < vertexStoreSize; ++idx) {
                        auto* localVertex{ storage->operator[](idx) };
                        if (localVertex && localVertex->id == vertexId) {
                            vertex = localVertex;
                            break;
                        }
                    }

                    if (!vertex) {
                        {
                            std::stringstream ss;
                            ss << "(transferGossip): [ERROR]: Unable to find vertex with VertexId=" << vertexId.ToString()
                                << " inside vertex store of rank=" << upcxx::rank_me() << std::endl;
                            logMsg(ss.str());
                        }

                        return false;
                    }

                    vertex->data = data;
                    return true;
                }, storage, vertexId, data);
        };

        // using DistFeaturesVector = upcxx::dist_object<std::vector<upcxx::future<>>>;
        using DistFeaturesVector = std::vector<upcxx::future<bool>>;
        DistFeaturesVector neighbourhoodFeatures{ {} };

        auto broadcastGossip = [&transferGossip, &neighbourhoodFeatures](GraphStorageType& storage, typename Vertex::Id vertexId, typename Vertex::Data data, upcxx::dist_object<std::vector<VertexId>>& visited)
        {
            // TODO: VertexId may not always directly map to vertex store index, so we need to perform O(N) search here.
            const auto vertexStoreSize{ storage->size() };
            Vertex* vertex{ nullptr };
            for (std::size_t idx{ 0 }; idx < vertexStoreSize; ++idx) {
                auto* localVertex{ storage->operator[](idx) };
                if (localVertex && localVertex->id == vertexId) {
                    vertex = localVertex;
                    break;
                }
            }

            std::vector<typename Vertex::Id> neighIds;
            if (!vertex) {
                {
                    std::stringstream ss;
                    ss << "(broadcastGossip): [ERROR]: Unable to find vertex with VertexId=" << vertexId.ToString()
                        << " inside vertex store of rank=" << upcxx::rank_me() << std::endl;
                    logMsg(ss.str());
                }
                return neighIds;
            }

            // Update current vertex data
            // logMsg("[VAGAG]: visited.size()=" + std::to_string(visited->size()));
            vertex->data = data;
            visited->push_back(vertexId);

            for (auto neighbour : vertex->neighbourhood) {
                // neighbourhoodFeatures.push_back(transferGossip(storage, neighbour.id, data));
                neighIds.push_back(neighbour.id);
            }

            return neighIds;
        };

        const auto randomVertex{ getRandomVertexFromLocalStore(graph.GetGraphStorage()) };
        if (!randomVertex) {
            {
                std::stringstream ss;
                ss << "(BroadcastGossip): [ERROR]: Unable to get random vertex from graph" << std::endl;
                logMsg(ss.str());
            }
            return;
        }

        typename Vertex::Id nextVertexId = randomVertex->id;
        while (getVisitedSize(visited) != localStorage.size()) {
            std::vector<upcxx::future<bool>> frontline{ };
            const auto vertexIdParent{ PGASGraph::GetVertexParent<Vertex>(nextVertexId, graph.GetVerticesPerRank()) };
            auto ids = upcxx::rpc(
                vertexIdParent,
                broadcastGossip,
                graph.GetGraphStorage(), nextVertexId, data, visited).wait();

            upcxx::barrier();

            for (auto id : ids) {
                frontline.push_back(transferGossip(graph.GetGraphStorage(), id, data));
            }

            for (auto& feature : frontline) {
                feature.wait();
            }

            upcxx::barrier();
        }

        upcxx::barrier();

        {
            std::stringstream ss;
            const auto stopTime = std::chrono::high_resolution_clock::now();
            ss << "[debug]: Finished broadcast-based gossip algorithm" << ". Gossiping took " << std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count() << "ms" << std::endl;
            logMsg(ss.str());
        }
    }
}
