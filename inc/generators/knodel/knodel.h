#pragma once

#include <cstdint>
#include <utility>
#include <vector>

namespace PGASGraph::Generators::Knodel
{

using Id = std::uint64_t;

struct VertexId
{
    using Id = Id;

    Id partition;
    Id id;

    bool operator==(const VertexId& other)
    {
        return partition == other.partition && id == other.id;
    }

    bool operator!=(const VertexId& other)
    {
        return !(*this == other);
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

    VertexId id;
    VertexData data;
    std::vector<Neighbour> neighbourhood;

    bool operator==(const Vertex& other)
    {
        return id == other.id;
    }

    bool operator!=(const Vertex& other)
    {
        return !(*this == other);
    }

    bool hasNeighbour(const VertexId& id)
    {
        return std::find(neighbourhood.begin(), neighbourhood.end(), id) != neighbourhood.end();
    }

    void addNeighbour(const EdgeData& data, const VertexId& id) {
        neighbourhood.push_back(Neighbour{data, id});
    }
};

template <typename EdgeData> struct Edge
{
    VertexId from;
    VertexId to;
    EdgeData data;

    bool isLoop() const
    {
        return from == to;
    }
};

} // namespace PGASGraph::Generators::Knodel
