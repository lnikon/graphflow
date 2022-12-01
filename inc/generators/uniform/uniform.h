#pragma once

#include <pgas-graph/pgas-graph.h>
#include <upcxx/upcxx.hpp>

#include <vector>
#include <string>
#include <algorithm>
#include <utility>
#include <random>
#include <cmath>
#include <iostream>
#include <cassert>
#include <sstream>

#include <cstdint>

namespace PGASGraph::Generators::Uniform
{

using Id = std::uint64_t;

struct VertexId
{
    Id id{0};
    bool isNull{false};

    VertexId() {}

    explicit VertexId(const Id id, const bool isNull = false)
        : id(id), isNull(isNull) {}

    VertexId(const VertexId& vertexId) = default;

    std::size_t UniversalId() const
    {
        return id;
    }

    bool operator==(const VertexId& other) const
    {
        return UniversalId() == other.UniversalId();
    }

    bool operator!=(const VertexId& other) const
    {
        return !(*this == other);
    }

    std::string ToString() const {
        return "id=" + std::to_string(id);
    }
};

template <typename EdgeData> struct Edge;

template <typename VertexId, typename VertexData, typename EdgeData> struct Neighbour
{
    EdgeData data;
    VertexId id;
};

// TODO: Consider disabling EdgeData based on graph configuration
template <typename VertexData, typename EdgeData> struct Vertex
{
    using Id = VertexId;
    using Data = VertexData;

    VertexId id;
    VertexData data;
    // TODO: Consider to use std::set<>
    std::vector<Neighbour<Id, VertexData, EdgeData>> neighbourhood;

    explicit Vertex(const VertexId id)
        : id(id) {}

    bool operator==(const Vertex& other)
    {
        return id == other.id;
    }

    bool operator!=(const Vertex& other)
    {
        return !(*this == other);
    }

    bool HasNeighbour(const VertexId& vertexId)
    {
        return std::find_if(neighbourhood.begin(),
                            neighbourhood.end(),
                            [vertexId](const auto& neighbour)
                            { return neighbour.id == vertexId; }) != neighbourhood.end();
    }

    void AddNeighbour(const EdgeData& data, const VertexId& id)
    {
        //std::cout << "(addNeighbour): id.partition=" << id.partition << ", id.id=" << id.partition
        //          << ", data=" << data << std::endl;

        neighbourhood.push_back(Neighbour<Id, VertexData, EdgeData>{data, id});
    }

    std::string ToString() const {
        return "id=" + id.ToString() + ", data=" + std::to_string(data);
    }
};

template <typename EdgeData> struct Edge
{
    VertexId from;
    VertexId to;
    EdgeData data;

    bool IsLoop() const
    {
        return from == to;
    }

    Edge Invert() const
    {
        return Edge{to, from, data};
    }

    std::string ToString() const
    {
        return "from=" + from.ToString() + ", edge=" + to.ToString() + ", data=" + std::to_string(data);
    }
};

template <template<typename, typename> class Graph, typename Vertex, typename Edge>
size_t
Generate(const size_t vertexCount, double percentage, Graph<Vertex, Edge>& graph)
{
    using PId = typename Vertex::Id;
    using Rank = upcxx::intrank_t;

    std::stringstream ss;
    ss << "[debug]: Starting generation of uniform random graph with " << vertexCount << " vertices and " << percentage << " density" << std::endl;
    logMsg(ss.str());
    ss.flush();

    const auto startTime = std::chrono::high_resolution_clock::now();

    const auto rank_me{upcxx::rank_me()};
    const auto rank_n{upcxx::rank_n()};

    const auto minId = rank_me * vertexCount;
    const auto maxId = (rank_me + 1) * vertexCount - 1;

    std::list<PId> unconnected;
    std::vector<PId> connected;
    connected.reserve(vertexCount);

    size_t edges{0};

    std::random_device rnd;
    std::mt19937 gen{rnd()};

    connected.push_back(PId(minId));
    for (auto i = minId + 1; i <= maxId; ++i)
    {
        unconnected.push_back(PId(i));
    }

    using dist_weight_t = upcxx::dist_object<size_t>;
    dist_weight_t weight{1};
    auto genWeight = [](dist_weight_t& weight)
    {
        return upcxx::rpc(
                   0,
                   [](dist_weight_t& weight)
                   {
                       auto res = *weight;
                       *weight += 1;
                       return res;
                   },
                   weight)
            .wait();
    };

    auto start = std::chrono::steady_clock::now();
    while (!unconnected.empty())
    {
        const auto temp1 =
            std::uniform_int_distribution<unsigned long long>(0, connected.size())(gen);
        const auto temp2 =
            std::uniform_int_distribution<unsigned long long>(0, unconnected.size() - 1)(gen);

        PId u{connected[temp1]};
        if (u.UniversalId() < minId || u.UniversalId() > maxId)
        {
            continue;
        }

        auto unconnectedItemIt{unconnected.begin()};
        std::advance(unconnectedItemIt, temp2);
        assert(unconnectedItemIt != unconnected.end());
        auto unconnectedItem{*unconnectedItemIt};
        PId v{unconnectedItem};
        if (v.UniversalId() < minId || v.UniversalId() > maxId)
        {
            continue;
        }

        if (u != v)
        {
            //logMsg("[DEBUG]: Adding edge between u=" + std::to_string(u.id) + " v=" + std::to_string(v.id));
            if (graph.AddEdge({u, v, genWeight(weight)}))
            {
                edges++;
            }
        }

        unconnected.erase(unconnectedItemIt);
        connected.push_back(v);
    }

    auto end = std::chrono::steady_clock::now();

    const size_t extraEdges{static_cast<size_t>(
        static_cast<double>((vertexCount * vertexCount) / 2) * (percentage / 100.0))};

    start = std::chrono::steady_clock::now();
    auto copyExtraEdges{extraEdges};
    size_t edgesInsideCurrentComponent{0};
    while (copyExtraEdges != 0)
    {
        PId u(std::uniform_int_distribution<unsigned long long>(minId, maxId)(gen));
        PId v(std::uniform_int_distribution<unsigned long long>(minId, maxId)(gen));
        if (u != v)
        {
            if (graph.AddEdge({u, v, genWeight(weight)}))
            {
                edgesInsideCurrentComponent++;
                edges++;
            }
            --copyExtraEdges;
        }
    }
    end = std::chrono::steady_clock::now();
    // PGASGraph::logMsg("Connected component interior generation time: " +
    //                   std::to_string(std::chrono::duration<double>(end - start).count()));
    // PGASGraph::logMsg("Edges inside current component: " +
    //                   std::to_string(edgesInsideCurrentComponent));

    auto genIdForPartition = [&gen](const auto rank, const auto vertexCount)
    {
        auto minId = rank * vertexCount;
        auto maxId = (rank + 1) * vertexCount - 1;
        return std::uniform_int_distribution<unsigned long long>(minId, maxId)(gen);
    };

    start = std::chrono::steady_clock::now();
    size_t edgesWithOtherComponents{0};
    for (Rank r1 = 0; r1 < rank_n - 1; ++r1)
    {
        for (Rank r2 = r1 + 1; r2 < rank_n; ++r2)
        {
            copyExtraEdges = rank_n + extraEdges;
            while (copyExtraEdges != 0)
            {
                const PId idR1{genIdForPartition(r1, vertexCount)};
                const PId idR2{genIdForPartition(r2, vertexCount)};
                if (idR1 != idR2)
                {
                    if (graph.AddEdge({idR1, idR2, genWeight(weight)}))
                    {
                        edgesWithOtherComponents++;
                        edges++;
                    }
                    --copyExtraEdges;
                }
            }
        }
    }
    // PGASGraph::logMsg("Connected component exterior generation time: " +
    //                   std::to_string(std::chrono::duration<double>(end - start).count()));
    // PGASGraph::logMsg("Edges with other components: " +
    // std::to_string(edgesWithOtherComponents));
    upcxx::barrier();

    end = std::chrono::steady_clock::now();
    const auto stopTime = std::chrono::high_resolution_clock::now();
    ss << "[debug]: Finished generation of uniform random graph" << std::endl;
    ss << "[debug]: Generation took " << std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count() << "ms" << std::endl;
    logMsg(ss.str());

    return edges;
}
} // namespace PGASGraph::Generators::Knodel
