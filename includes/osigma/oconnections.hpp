#ifndef OCONNECTIONS_HPP_
#define OCONNECTIONS_HPP_

#include <cstdarg>
#include <string>
#include <tuple>
#include <vector>

namespace ograph {

template <typename TId, typename TValue, typename... TFeatures>
class OConnections {

    typedef std::tuple<std::vector<TFeatures>...> TFeaturesTuple;

public:
    std::vector<TId> m_from;
    std::vector<TId> m_to;
    std::vector<TValue> m_values;
    TFeaturesTuple m_features;

    explicit OConnections(std::vector<TId> from, std::vector<TId> to, std::vector<TValue> values, std::vector<TFeatures>... features)
        : m_from(from)
        , m_to(to)
        , m_values(values)
        , m_features(TFeaturesTuple(features...))
    {
    }

    std::string describe() const
    {

        return "OConnections(from, to, values + " + std::to_string(std::tuple_size<TFeaturesTuple>()) + " features of " + std::to_string(m_from.size()) + " connections)";
    }
};

template <typename TId, typename TValue, typename TZIndex, typename... TFeatures>
class OSpatialConnections : public OConnections<TId, TValue, TFeatures...> {

    typedef std::tuple<std::vector<TFeatures>...> TFeaturesTuple;

public:
    std::vector<TZIndex> m_z_index;

    explicit OSpatialConnections(std::vector<TId> from, std::vector<TId> to, std::vector<TValue> values, std::vector<TZIndex> z_index, std::vector<TFeatures>... features)
        : OConnections<TId, TValue, TFeatures...>(from, to, values, features...)
        , m_z_index(z_index)
    {
    }
    std::string describe() const
    {
        int a = this->m_from.size();
        return "OSpatialConnections(from, to, values, z_index + " + std::to_string(std::tuple_size<TFeaturesTuple>()) + " features of " + std::to_string(this->m_from.size()) + " connections)";
    }
};
}

#endif