#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <utility>
#include <cmath>
#include <iostream>

#include <cstdint>

namespace PGASGraph::Generators::Knodel
{

using Id = std::uint64_t;

struct VertexId
{
    Id partition{0};
    Id id{0};
    bool isNull{false};

    std::size_t UniversalId() const
    {
        std::cout << "(VertexId::UniversalId): universalId=" << ToString() << std::endl;
        return partition * 10 + id;
    }

    bool operator==(const VertexId& other) const
    {
        return partition == other.partition && id == other.id;
    }

    bool operator!=(const VertexId& other) const
    {
        return !(*this == other);
    }

    std::string ToString() const {
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
size_t Generate(const size_t vertexCount, const size_t delta, Graph<Vertex, Edge>& graph)
{
    size_t edgeCount{0};
    // for (std::uint64_t i = 0; i < 2; ++i)
    {
        for (std::uint64_t j = 0; j < vertexCount / 2; ++j)
        {
            const typename Vertex::Id from{0, j};
            for (std::uint64_t d = 0; d < delta; ++d)
            {
                const auto k{static_cast<std::size_t>((j + static_cast<std::size_t>(std::pow(2, d)) - 1) % (vertexCount / 2))};
                const typename Vertex::Id to{1, k};
                if (graph.AddEdge({from, to, 0}))
                {
                    edgeCount++;
                }
            }
        }
    }

    return edgeCount;
}

} // namespace PGASGraph::Generators::Knodel
