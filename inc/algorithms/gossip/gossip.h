#pragma once

#include <pgas-graph/pgas-graph.h>

#include <sstream>

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

        std::stringstream ss;
        ss << "[debug]: Starting randomized push-based gossip algorithm from vertex with Id " << vertexId.ToString() << " and data " << data << std::endl;
        logMsg(ss.str());
        ss.flush();

        const auto startTime = std::chrono::high_resolution_clock::now();

        const auto localStorage{ graph.GetGraphStorage().fetch(upcxx::rank_me()).wait() };
        logMsg("localStorage=" + std::to_string(localStorage.size()));

        // upcxx::barrier();

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
                logMsg("(PushRandomizedGossip::getRandomVertexFromLocalStore): storage->size()=" + std::to_string(storage->size()));
                const auto size{ storage->size() };
                std::size_t notNullCount{ 0 };
                for (std::size_t idx{ 0 }; idx < size; ++idx) {
                    if (storage->operator[](idx)) {
                        notNullCount++;
                    }
                }
                logMsg("(PushRandomizedGossip::getRandomVertexFromLocalStore): notNullCount=" + std::to_string(notNullCount));
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

        const auto randomVertex{ getRandomVertexFromLocalStore(graph.GetGraphStorage()) };
        if (!randomVertex) {
            return;
        }

        typename Vertex::Id nextVertexId = randomVertex->id;
        while (getVisitedSize(visited) != localStorage.size()) {
            const auto vertexIdParent{ PGASGraph::GetVertexParent<Vertex>(nextVertexId, graph.GetVerticesPerRank()) };
            nextVertexId = upcxx::rpc(
                vertexIdParent,
                pushRandomizedGossip,
                graph.GetGraphStorage(), nextVertexId, data, visited).wait();
        }

        upcxx::barrier();

        auto end = std::chrono::steady_clock::now();
        const auto stopTime = std::chrono::high_resolution_clock::now();

        ss << "[debug]: Finished randomized push-based gossip algorithm" << std::endl;
        logMsg(ss.str());
        ss.flush();
        ss << "[debug]: Gossiping took " << std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime).count() << "ms" << std::endl;
        logMsg(ss.str());
    }

    template <typename Vertex, typename Edge>
    void BroadcastGossip(Graph<Vertex, Edge>& graph, typename Vertex::Id vertexId, typename Vertex::Data data)
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

        std::stringstream ss;
        ss << "[debug]: Starting broadcast-based gossip algorithm from vertex with Id " << vertexId.ToString() << " and data " << data << std::endl;
        logMsg(ss.str());
        ss.flush();

        const auto startTime = std::chrono::high_resolution_clock::now();

        const auto localStorage{ graph.GetGraphStorage().fetch(upcxx::rank_me()).wait() };
        logMsg("localStorage=" + std::to_string(localStorage.size()));

        // upcxx::barrier();

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
                logMsg("(BroadcastGossip::getRandomVertexFromLocalStore): storage->size()=" + std::to_string(storage->size()));
                const auto size{ storage->size() };
                std::size_t notNullCount{ 0 };
                for (std::size_t idx{ 0 }; idx < size; ++idx) {
                    if (storage->operator[](idx)) {
                        notNullCount++;
                    }
                }
                logMsg("(BroadcastGossip::getRandomVertexFromLocalStore): notNullCount=" + std::to_string(notNullCount));
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
                        if (localVertex) {
                            if (localVertex->id == vertexId) {
                                vertex = localVertex;
                                break;
                            }
                        }
                    }

                    if (!vertex) {
                        std::cerr << "[ERROR]: Unable to find vertex with VertexId=" << vertexId.ToString()
                            << " inside vertex store of rank=" << upcxx::rank_me() << std::endl;
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
            // const auto localVertexStore = graph->fetch(upcxx::rank_me()).wait();

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

            std::vector<typename Vertex::Id> neighIds;
            if (!vertex) {
                std::cerr << "[ERROR]: Unable to find vertex with VertexId=" << vertexId.ToString()
                    << " inside vertex store of rank=" << upcxx::rank_me() << std::endl;
                return neighIds;
            }

            // Update current vertex data
            logMsg("[VAGAG]: visited.size()=" + std::to_string(visited->size()));
            vertex->data = data;
            visited->push_back(vertexId);

            for (auto neighbour : vertex->neighbourhood) {
                // neighbourhoodFeatures.push_back(transferGossip(storage, neighbour.id, data));
                neighIds.push_back(neighbour.id);
            }

            //for (auto& feature : neighbourhoodFeatures) {
            //    feature.then([](){});
            //}

            return neighIds;
        };

        const auto randomVertex{ getRandomVertexFromLocalStore(graph.GetGraphStorage()) };
        if (!randomVertex) {
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

            //auto localFeatures{ neighbourhoodFeatures.fetch(upcxx::rank_me()).wait() };
            for (auto& feature : neighbourhoodFeatures) {
                //logMsg("asdfas");
                //feature.wait();
            }

            for (auto id : ids) {
                frontline.push_back(transferGossip(graph.GetGraphStorage(), id, data));
            }

            for (auto& feature : frontline) {
                feature.wait();
            }

            upcxx::barrier();


            //for (const auto id : neighbourhoodIds) {
            //    const auto neighbourParentRank{ PGASGraph::GetVertexParent<Vertex>(id, graph.GetVerticesPerRank()) };
            //    const auto neighbourFuture = upcxx::rpc(neighbourParentRank, broadcastGossip, graph.GetGraphStorage(), id, data, visited);
            //}
        }

        upcxx::barrier();

        auto end = std::chrono::steady_clock::now();
        const auto stopTime = std::chrono::high_resolution_clock::now();

        ss << "[debug]: Finished broadcast-based gossip algorithm" << std::endl;
        logMsg(ss.str());
        ss.flush();
        ss << "[debug]: Gossiping took " << std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime).count() << "ms" << std::endl;
        logMsg(ss.str());
    }
}
