#ifndef ONODES_HPP_
#define ONODES_HPP_

#include <cstdarg>
#include <string>
#include <tuple>
#include <vector>

namespace ograph {

template <typename... TFeatures>
class ONodes {
    typedef std::tuple<std::vector<TFeatures>...> TFeaturesTuple;

public:
    TFeaturesTuple m_features;

    explicit ONodes(std::vector<TFeatures>... features)
        : m_features(TFeaturesTuple(features...))
    {
    }
    std::string describe() const
    {

        return "ONodes(" + std::to_string(std::tuple_size<TFeaturesTuple>()) + " features of " + std::to_string(std::get<0>(m_features).size()) + " nodes)";
    }
};

template <typename TCoordinates, typename TZIndex, typename... TFeatures>
class OSpatialNodes : public ONodes<TFeatures...> {

    typedef std::tuple<std::vector<TFeatures>...> TFeaturesTuple;

public:
    std::vector<TCoordinates> m_x_coordinates;
    std::vector<TCoordinates> m_y_coordinates;
    std::vector<TZIndex> m_z_index;

    explicit OSpatialNodes(
        std::vector<TCoordinates> x_coordinates, std::vector<TCoordinates> y_coordinates,
        std::vector<TZIndex> z_index, std::vector<TFeatures>... features)
        : ONodes<TFeatures...>(features...)
        , m_x_coordinates(x_coordinates)
        , m_z_index(z_index)
    {
    }
    std::string describe() const
    {

        return "OSpatialNodes(x, y + " + std::to_string(std::tuple_size<TFeaturesTuple>()) + " features of " + std::to_string(m_x_coordinates.size()) + " nodes)";
    }
};
}

#endif