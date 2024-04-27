#ifndef DECAYING_MAX_HEAP_HPP_
#define DECAYING_MAX_HEAP_HPP_

#include <map>
#include <string>
#include <tuple>
#include <vector>

namespace clustering {

template <
    typename TValue,
    typename... TKeys>
class DecayingMaxHeap {
    typedef std::tuple<TKeys...> TKeysTuple;
    typedef std::tuple<TKeys..., TValue> TKeysValueTuple;
    typedef std::tuple<std::vector<TKeys>...> TKeysVectorTuple;

public:
    TKeysVectorTuple m_heap_keys;
    std::vector<TValue> m_heap_values;
    std::map<TKeysTuple, size_t> m_node_position_map;

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

        siftdown(0, size() - 1, value, std::make_tuple(keys...));
    }

    void push(TKeysTuple keys_tuple, TValue value)
    {

        std::apply([&](auto&... keys) { push(keys..., value); },
            keys_tuple);
    }

    void push(TKeysValueTuple keys_value_tuple)
    {

        std::apply([&](auto&... key_values) { push(key_values...); },
            keys_value_tuple);
    }

    TKeysTuple top_key()
    {
        return key_at(0);
    }

    TValue top_value()
    {
        return value_at(0);
    }

    TKeysValueTuple top()
    {
        return std::apply([&](auto&... vectors) { return std::make_tuple(vectors[0]..., m_heap_values[0]); },
            m_heap_keys);
    }

    TKeysValueTuple pop()
    {

        size_t latest_id = size() - 1;
        TKeysTuple result_key = top_key();
        TKeysValueTuple result = top();

        remove_node_position_in_map(result_key);

        if (latest_id > 0) {

            assign(0, latest_id);

            reduce_size();

            siftup(0, size(), top_value(), top_key());
        } else {

            reduce_size();
        }

        return result;
    }

    TValue remove(TKeysTuple keys)
    {

        size_t latest_id = size() - 1;
        size_t position = get_node_position_in_map(keys);
        TKeysTuple output_key = key_at(position);
        TValue value = value_at(position);

        remove_node_position_in_map(keys);

        if (latest_id > 0) {

            assign(position, latest_id);

            reduce_size();

            siftup(position, size(), value_at(position), key_at(position));
        } else {

            reduce_size();
        }
    }
    TValue remove(TKeys... keys)
    {

        return remove(std::make_tuple(keys...));
    }

    void update_value(TKeysTuple keys, TValue new_value)
    {

        size_t position = get_node_position_in_map(keys);
        TValue old_value = value_at(position);
        m_heap_values[position] = new_value;

        if (new_value > old_value) {

            siftdown(0, position, new_value, keys);
        } else {

            siftup(position, size() - 1, new_value, keys);
        }
    }

    void update_value(TKeys... keys, TValue new_value)
    {

        update_value(std::make_tuple(keys...), new_value);
    }

    size_t size()
    {

        return m_heap_values.size();
    }

    std::string describe() const
    {

        return "DecayingMaxHeap(" + std::to_string(std::tuple_size<TKeysVectorTuple>()) + " keys and a value of " + std::to_string(m_heap_values.size()) + " heap elements)";
    }

    std::string describe_element(size_t element_id) const
    {

        TKeysTuple keys = std::apply([element_id](auto&... vectors) { return std::make_tuple(vectors[element_id]...); },
            m_heap_keys);

        std::string keys_str = std::apply([](auto&... values) { return ((std::to_string(values) + ", ") + ...); },
            keys);

        return "DecayingMaxHeapElement@" + std::to_string(element_id) + "(keys " + keys_str + "and a value of " + std::to_string(m_heap_values[element_id]) + ")";
    }

    // private:
    TKeysTuple key_at(size_t element_id)
    {

        return std::apply([element_id](auto&... vectors) { return std::make_tuple(vectors[element_id]...); },
            m_heap_keys);
    }

    TValue value_at(size_t element_id)
    {

        return m_heap_values[element_id];
    }

    void assign(size_t target, size_t source)
    {
        m_heap_values[target] = m_heap_values[source];

        [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            ([&]() {
                auto& vector = std::get<Is>(m_heap_keys);
                vector[target] = vector[source];
            }(),
                ...);
        }(std::index_sequence_for<TKeys...>());

        set_node_position_in_map(key_at(target), target);
    }

    void assign_value(size_t target, TValue value, TKeysTuple key)
    {

        m_heap_values[target] = value;

        [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            ([&]() {
                auto& vector = std::get<Is>(m_heap_keys);
                auto& val = std::get<Is>(key);
                vector[target] = val;
            }(),
                ...);
        }(std::index_sequence_for<TKeys...>());

        set_node_position_in_map(key, target);
    }

    void siftdown(size_t start, size_t position, TValue new_value, TKeysTuple new_key)
    {

        while (position > start) {

            size_t parent = (position - 1) >> 1;
            TValue parent_value = value_at(parent);

            if (new_value > parent_value) {
                assign(position, parent);
                position = parent;
            } else {

                break;
            }
        }

        assign_value(position, new_value, new_key);
    }

    void siftup(size_t position, size_t end, TValue new_value, TKeysTuple new_key)
    {
        size_t start = position;

        size_t child_position = 2 * position + 1;

        while (child_position < end) {

            size_t right_child_position = child_position + 1;

            if ((right_child_position < end) && !(m_heap_values[child_position] > m_heap_values[right_child_position])) {
                child_position = right_child_position;
            }

            assign(position, child_position);
            position = child_position;
            child_position = 2 * position + 1;
        }

        assign_value(position, new_value, new_key);

        siftdown(start, position, new_value, new_key);
    }

    void remove_node_position_in_map(TKeysTuple key)
    {
        m_node_position_map.erase(key);
    }

    size_t get_node_position_in_map(TKeysTuple key)
    {
        return m_node_position_map[key];
    }

    void set_node_position_in_map(TKeysTuple key, size_t position)
    {
        m_node_position_map[key] = position;
    }

    void reduce_size()
    {

        m_heap_values.pop_back();

        std::apply([](auto&... vectors) {
            (vectors.pop_back(), ...);
        },
            m_heap_keys);
    }
};
}

#endif