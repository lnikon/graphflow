#pragma once

#include <pgas-graph/pgas-graph.h>
#include <upcxx/upcxx.hpp>

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
    using Size = std::uint64_t;

    struct VertexId
    {
        Id partition{ 0 };
        Id id{ 0 };
        bool isNull{ false };
        Size shift{ 0 };

        VertexId() {}

        explicit VertexId(const Id partitionid, const Id id, const Size shift, const bool isNull = false)
            : partition(partitionid), id(id), shift(shift), isNull(isNull) {}

        VertexId(const VertexId& vertexId) = default;

        std::size_t UniversalId() const
        {
            const auto result{ partition * shift + id };
            //logMsg("(Knodel::UniversalId): id=" + std::to_string(result));
            return result;
        }

        bool operator==(const VertexId& other) const
        {
            //return partition == other.partition && id == other.id;
            return UniversalId() == other.UniversalId();
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

        explicit Vertex(const VertexId vertexId)
            : id(vertexId) {}

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
            return Edge{ to, from, data };
        }

        std::string ToString() const
        {
            return "from=" + from.ToString() + ", edge=" + to.ToString() + ", data=" + std::to_string(data);
        }
    };

    template <template<typename, typename> class Graph, typename Vertex, typename Edge>
    size_t Generate(const size_t totalVertexCount, const size_t vertexCount, const size_t delta, Graph<Vertex, Edge>& graph)
    {
        {
            std::stringstream ss;
            ss << "[debug]: Starting generation of knodel random graph with " << vertexCount << " vertices and " << delta << " vertex degree" << std::endl;
            logMsg(ss.str());
        }

        const auto startTime = std::chrono::high_resolution_clock::now();

        const auto rankMe{ upcxx::rank_me() };
        const auto rankN{ upcxx::rank_n() };
        const auto minId{ rankMe * vertexCount };
        const auto maxId{ (rankMe + 1) * vertexCount - 1 };

        size_t edgeCount{ 0 };
        {
            for (std::uint64_t j = minId; j < maxId; ++j)
            {
                const typename Vertex::Id from{ 0, j, totalVertexCount / 2 };
                for (std::uint64_t d = 0; d < delta; ++d)
                {
                    const auto k{ static_cast<std::size_t>((j + static_cast<std::size_t>(std::pow(2, d)) - 1) % ((vertexCount * rankN) / 2)) };
                    const typename Vertex::Id to{ 1, k, totalVertexCount / 2 };
                    if (graph.AddEdge({ from, to, 0 }))
                    {
                        edgeCount++;
                    }
                }
            }
        }

        {
            const auto stopTime = std::chrono::high_resolution_clock::now();
            std::stringstream ss;
            ss << "[debug]: Finished generation of knodel graph. Generation took " << std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count() << "ms" << std::endl;
            logMsg(ss.str());
        }

        return edgeCount;
    }
} // namespace PGASGraph::Generators::Knodel
