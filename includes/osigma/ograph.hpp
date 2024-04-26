#ifndef OGRAPH_HPP_
#define OGRAPH_HPP_

#include <string>
#include <vector>

#include <osigma/oconnections.hpp>
#include <osigma/onodes.hpp>

namespace ograph {

template <
    typename TId,
    typename TConnectionWeight,
    typename TCoordinates,
    typename TZIndex,
    typename... TNodeFeatures>
class OGraph {

public:
    OSpatialNodes<TCoordinates, TZIndex, TNodeFeatures...> m_nodes;
    OSpatialConnections<TId, TConnectionWeight, TZIndex> m_connections;

    explicit OGraph(
        OSpatialNodes<TCoordinates, TZIndex, TNodeFeatures...> nodes,
        OSpatialConnections<TId, TConnectionWeight, TZIndex> connections)
        : m_nodes(nodes)
        , m_connections(connections)
    {
    }

    int node_count()
    {

        return m_nodes.m_x_coordinates.size();
    }

    int connection_count()
    {

        return m_connections.m_from.size();
    }

    std::string describe() const
    {

        return "OGraph(with " + m_nodes.describe() + " and " + m_connections.describe() + ")";
    }
};
}

#endif