#ifndef DECAYING_MAX_HEAP_HPP_
#define DECAYING_MAX_HEAP_HPP_

#include <string>
#include <tuple>
#include <vector>

namespace clustering {

template <
    typename TValue,
    typename... TKeys>
class DecayingMaxHeap {
    typedef std::tuple<TKeys...> TKeysTuple;
    typedef std::tuple<std::vector<TKeys>...> TKeysVectorTuple;

public:
    TKeysVectorTuple m_heap_keys;
    std::vector<TValue> m_heap_values;

    explicit DecayingMaxHeap(size_t size)
    {

        m_heap_values.reserve(size);

        std::apply([size](auto&... vectors) { (vectors.reserve(size), ...); },
            m_heap_keys);
    }

    void push(TKeys... keys, TValue value)
    {

        std::apply([&](auto&... vectors) { (vectors.push_back(keys), ...); },
            m_heap_keys);

        m_heap_values.push_back(value);
    }

    std::string describe() const
    {

        return "DecayingMaxHeap(" + std::to_string(std::tuple_size<TKeysVectorTuple>()) + " keys and a value of " + std::to_string(m_heap_values.size()) + " heap elements)";
    }

    std::string show_element(size_t element_id) const
    {

        TKeysTuple keys = std::apply([element_id](auto&... vectors) { return std::make_tuple(vectors[element_id]...); },
            m_heap_keys);

        std::string keys_str = std::apply([](auto&... values) { return ((std::to_string(values) + ", ") + ...); },
            keys);

        return "DecayingMaxHeapElement@" + std::to_string(element_id) + "(keys " + keys_str + "and a value of " + std::to_string(m_heap_values[element_id]) + ")";
    }

private:
    void assign(size_t target, size_t source)
    {
    }

    void assign_value(size_t target, TKeys... keys, TValue value)
    {
    }
};
}

#endif