#pragma once

#include "../../pgasgraph.h"

#include <upcxx/upcxx.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstdint>

namespace PGASGraph::Generators::Knodel
{

using Id = std::uint64_t;
using Size = std::uint64_t;

struct VertexId
{
    Id partition{0};
    Id id{0};
    bool isNull{false};
    Size shift{0};

    VertexId()
    {
    }

    explicit VertexId(const Id partitionid,
                      const Id id,
                      const Size shift,
                      const bool isNull = false)
        : partition(partitionid)
        , id(id)
        , shift(shift)
        , isNull(isNull)
    {
    }

    VertexId(const VertexId& vertexId) = default;

    std::size_t UniversalId() const
    {
        const auto result{partition * shift + id};
        // logMsg("(Knodel::UniversalId): id=" + std::to_string(result));
        return result;
    }

    bool operator==(const VertexId& other) const
    {
        // return partition == other.partition && id == other.id;
        return UniversalId() == other.UniversalId();
    }

    bool operator!=(const VertexId& other) const
    {
        return !(*this == other);
    }

    std::string ToString() const
    {
        return "partition=" + std::to_string(partition) + ", id=" + std::to_string(id);
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

    explicit Vertex(const VertexId vertexId)
        : id(vertexId)
    {
    }

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
        neighbourhood.push_back(Neighbour<Id, VertexData, EdgeData>{data, id});
    }

    std::string ToString() const
    {
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
        return "from=" + from.ToString() + ", edge=" + to.ToString() +
               ", data=" + std::to_string(data);
    }
};

template <template <typename, typename> class Graph, typename Vertex, typename Edge>
size_t Generate(size_t totalVertexCount,
                size_t vertexCountPerRank,
                const size_t delta,
                Graph<Vertex, Edge>& graph)
{
    {
        std::stringstream ss;
        ss << "[debug]: Starting generation of knodel random graph with " << vertexCountPerRank
           << " vertices and " << delta << " vertex degree" << std::endl;
        //logMsg(ss.str());
    }

    totalVertexCount /= 2;
    vertexCountPerRank = (totalVertexCount + upcxx::rank_n() - 1) / upcxx::rank_n();

    const auto startTime = std::chrono::high_resolution_clock::now();

    const auto rankMe{upcxx::rank_me()};
    const auto rankN{upcxx::rank_n()};
    auto minId{rankMe * vertexCountPerRank};
    auto maxId{(rankMe + 1) * vertexCountPerRank - 1};
    const auto deltaPerRank{static_cast<std::size_t>(std::log2(vertexCountPerRank))};

    size_t edgeCount{0};
    {
        {
            std::stringstream ss;
            ss << "[debug]: minId=" << minId << ", maxId=" << maxId
               << ", maxId % (totalVertexCount / 2)=" << maxId % (totalVertexCount / 2)
               << ", maxId/2=" << maxId / 2 << std::endl;
            //logMsg(ss.str());
        }

        // for (std::uint64_t j = minId; j < (minId + vertexCountPerRank)/2; ++j)
        //% static_cast<std::size_t>(std::ceil((totalVertexCount / 2))) + 1
        // for (std::uint64_t j = minId; j <= (minId + vertexCountPerRank / 2 - 1) %
        // (totalVertexCount / 2); ++j)
        for (std::uint64_t j = minId; j <= maxId % (2 * totalVertexCount); ++j)
        {
            {
                // std::stringstream ss;
                // ss << "[debug]: j=" << j << std::endl;
                // logMsg(ss.str());
            }

            const typename Vertex::Id from{0, j, totalVertexCount};
            for (std::uint64_t d = 0; d < delta; ++d)
            {
                // const auto k{ static_cast<std::size_t>((j + static_cast<std::size_t>(std::pow(2,
                // d)) - 1) % ((totalVertexCount) / 2)) };
                const auto k{
                    static_cast<std::size_t>((j + static_cast<std::size_t>(std::pow(2, d)) - 1)) %
                    (static_cast<std::size_t>(std::ceil(totalVertexCount)))};
                const typename Vertex::Id to{1, k, totalVertexCount};
                if (graph.AddEdge({from, to, 0}))
                {
                    if (upcxx::rank_me() == 0 || upcxx::rank_me() == 1)
                    {
                        {
                            // std::stringstream ss;
                            // ss << "[debug]: im here" << std::endl;
                            // logMsg(ss.str());
                        }
                    }
                    edgeCount++;
                }
            }
        }
    }

    {
        const auto stopTime = std::chrono::high_resolution_clock::now();
        std::stringstream ss;
        ss << "[debug]: Finished generation of knodel graph. Generation took "
           << std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count()
           << "ms" << std::endl;
        //logMsg(ss.str());
    }

    return edgeCount;
}
} // namespace PGASGraph::Generators::Knodel
