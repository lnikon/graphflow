#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <utility>
#include <iostream>

#include <cstdint>

namespace PGASGraph::Generators::Knodel
{

using Id = std::uint64_t;

struct VertexId
{
    Id partition;
    Id id;

    std::size_t UniversalId() const
    {
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
        std::cout << "(addNeighbour): id.partition=" << id.partition << ", id.id=" << id.partition
                  << ", data=" << data << std::endl;

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

} // namespace PGASGraph::Generators::Knodel
