#ifndef CLAUSET_NEWMAN_MOORE_HPP_
#define CLAUSET_NEWMAN_MOORE_HPP_

#define PRINT_FREQUENCY_DQ 10000
#define PRINT_FREQUENCY_DQH 1000

#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include <ginv/clustering/decaying_max_heap.hpp>
#include <osigma/ograph.hpp>

#include <cstdio>
#include <iostream>

namespace clustering {

template <typename TId>
void display_communities(std::vector<std::vector<TId>> communities, bool show_size = true, std::ostream& stream = std::cout)
{

    std::sort(communities.begin(), communities.end(), [](auto a, auto b) { return a.size() > b.size(); });

    stream << "communities {";
    int count = 1;
    int non_zero_size = 0;

    for (int i = 0; i < communities.size(); i++) {

        if (show_size) {

            if (communities[i].size() > 0) {

                non_zero_size++;
            }

            if (i + 1 < communities.size() && communities[i].size() == communities[i + 1].size()) {
                count++;
            } else {

                stream << "s" << communities[i].size() << (count > 1 ? "x" + std::to_string(count) : "") << (i == communities.size() - 1 ? "" : ", ");
                count = 1;
            }

        } else {

            stream << "[";

            for (int q = 0; q < communities[i].size(); q++) {

                stream << communities[i][q] << (q == communities[i].size() - 1 ? "" : ", ");
            }

            stream << "]" << (i == communities.size() - 1 ? "" : ", ");
        }
    }

    stream << "}";

    if (show_size) {

        stream << " [" << non_zero_size << "]";
    }

    stream << std::endl;
}

template <
    typename TKey,
    typename TValue>
std::set<TKey> map_key_set(std::map<TKey, TValue> map)
{
    std::set<TKey> result;

    for (auto const& [key, value] : map) {

        result.insert(key);
    }

    return result;
}

template <
    typename TQ,
    typename TId,
    typename TConnectionWeight,
    typename TCoordinates,
    typename TZIndex,
    typename... TNodeFeatures>
std::vector<std::vector<TId>> greedy_modularity_communities(
    ograph::OGraph<
        TId,
        TConnectionWeight,
        TCoordinates,
        TZIndex,
        TNodeFeatures...>
        graph,
    TQ resolution = 1.0f,
    size_t cutoff = 1,
    bool verbose = false,
    TQ negative_infinity = -2605)
{

    typedef std::vector<std::vector<TId>> Communities;
    typedef std::vector<TQ> VectorQ;
    typedef std::map<TId, TQ> MapQ;
    typedef std::map<TId, DecayingMaxHeap<TQ, TId>> MapHeapQ;
    typedef std::map<TId, MapQ> DeltaQ;
    typedef std::set<TId> IdSet;

    Communities communities(graph.node_count());

    auto create_normal_weighted_degrees = [&]() {
        VectorQ result(graph.node_count());
        TQ m = 0;

        for (int i = 0; i < graph.connection_count(); i++) {

            TId from = graph.m_connections.m_from[i];
            TId to = graph.m_connections.m_to[i];

            if (from == to) {

                continue;
            }

            TConnectionWeight weight = graph.m_connections.m_values[i];

            result[from] += weight;
            result[to] += weight;

            m += weight;
        }

        TQ reverse_m = 1 / m;

        for (int i = 0; i < graph.node_count(); i++) {

            result[i] /= 2 * m;
        }

        return std::make_tuple(result, reverse_m);
    };

    auto a_reverse_m = create_normal_weighted_degrees();
    VectorQ a = std::get<0>(a_reverse_m);
    TQ reverse_m = std::get<1>(a_reverse_m);

    // std::cout << "as " << a.size() << " rm " << reverse_m << std::endl;

    auto create_delta_q = [&]() {
        DeltaQ result;

        for (size_t i = 0; i < graph.node_count(); i++) {

            result[i] = MapQ();
        }

        for (int i = 0; i < graph.connection_count(); i++) {

            if (verbose && (i % PRINT_FREQUENCY_DQ == 0 || i == graph.connection_count() - 1)) {
                std::printf("\rcreate_delta_q %.2f%%", i * 100.0f / graph.connection_count());
            }

            TId from = graph.m_connections.m_from[i];
            TId to = graph.m_connections.m_to[i];

            if (from == to) {

                continue;
            }

            TConnectionWeight weight = graph.m_connections.m_values[i];

            result[from][to] += weight;
            result[to][from] += weight;
        }

        for (auto const& [from, map] : result) {

            for (auto& [to, value] : map) {

                result[from][to] = reverse_m * result[from][to] - resolution * 2 * a[from] * a[to];
            }
        }

        return result;
    };

    if (verbose) {
        std::cout << std::endl;
    }

    DeltaQ delta_q = create_delta_q();

    auto create_heaps = [&]() {
        MapHeapQ delta_q_heaps;
        DecayingMaxHeap<TQ, TId, TId> total_heap(graph.node_count());

        for (int i = 0; i < graph.node_count(); i++) {

            if (verbose && (i % PRINT_FREQUENCY_DQH == 0 || i == graph.connection_count() - 1)) {
                std::printf("\rcreate_heaps %.2f%%", i * 100.0f / graph.node_count());
            }

            DecayingMaxHeap<TQ, TId> heap(delta_q[i].size());

            for (auto const& [to, value] : delta_q[i]) {

                heap.push(to, value);
            }

            delta_q_heaps.try_emplace(i, heap);

            if (heap.size() > 0) {

                auto top = heap.top();

                total_heap.push(i, std::get<0>(top), std::get<1>(top));
            }
        }

        return std::make_tuple(delta_q_heaps, total_heap);
    };

    auto all_heaps = create_heaps();
    MapHeapQ delta_q_heaps = std::get<0>(all_heaps);
    DecayingMaxHeap<TQ, TId, TId> total_heap = std::get<1>(all_heaps);

    for (int i = 0; i < graph.node_count(); i++) {

        communities[i] = std::vector { i };
    }

    auto step = [&]() {
        if (total_heap.size() > 1) {

            auto [u, v, top_delta_q] = total_heap.pop();

            if (top_delta_q < 0) {

                return top_delta_q;
            }

            auto merge_u_into_v = [&]() {
                auto update_heap_tops_by_removing_u = [&]() {
                    delta_q_heaps.at(u).pop();

                    if (delta_q_heaps.at(u).size() > 0) {

                        auto [key, dq] = delta_q_heaps.at(u).top();
                        total_heap.push(u, key, dq);
                    }

                    if (std::get<0>(delta_q_heaps.at(v).top_key()) == u) {

                        total_heap.remove(v, u);
                        delta_q_heaps.at(v).remove(u);

                        if (delta_q_heaps.at(v).size() > 0) {

                            auto [key, dq] = delta_q_heaps.at(v).top();
                            total_heap.push(v, key, dq);
                        }
                    } else {

                        delta_q_heaps.at(v).remove(u);
                    }
                };

                update_heap_tops_by_removing_u();
                communities[v].insert(communities[v].end(), communities[u].begin(), communities[u].end());
                communities[u].clear();

                IdSet u_neighbours = map_key_set(delta_q[u]);

                auto update_delta_q_for_affected_commuinities = [&]() {
                    IdSet v_neighbours = map_key_set(delta_q[v]);

                    IdSet all_neighbours(u_neighbours);
                    all_neighbours.insert(v_neighbours.begin(), v_neighbours.end());
                    all_neighbours.erase(u);
                    all_neighbours.erase(v);

                    IdSet common_neighbours;
                    std::set_intersection(
                        u_neighbours.begin(), u_neighbours.end(),
                        v_neighbours.begin(), v_neighbours.end(),
                        std::inserter(common_neighbours, common_neighbours.begin()));

                    for (auto w : all_neighbours) {

                        TQ dq_vw;

                        bool v_has_w = v_neighbours.find(w) != v_neighbours.end();

                        if (common_neighbours.find(w) != common_neighbours.end()) {
                            dq_vw = delta_q.at(v).at(w) + delta_q.at(u).at(w);
                        } else if (v_has_w) {
                            dq_vw = delta_q.at(v).at(w) - resolution * 2 * a[u] * a[w];
                        } else {
                            dq_vw = delta_q.at(u).at(w) - resolution * 2 * a[v] * a[w];
                        }

                        auto update_elements = [&](TQ r, TQ c) {
                            delta_q[r][c] = dq_vw;

                            std::tuple<TId, TQ> d_old_max;
                            bool has_d_old_max = false;

                            if (delta_q_heaps.at(r).size() > 0) {
                                has_d_old_max = true;
                                d_old_max = delta_q_heaps.at(r).top();
                            }

                            if (v_has_w) {
                                delta_q_heaps.at(r).update_value(c, dq_vw);
                            } else {
                                delta_q_heaps.at(r).push(c, dq_vw);
                            }

                            if (has_d_old_max) {

                                auto r_max = delta_q_heaps.at(r).top();

                                if ((std::get<0>(d_old_max) != std::get<0>(r_max)) || (std::get<1>(d_old_max) != std::get<1>(r_max))) {

                                    total_heap.remove(r, std::get<0>(d_old_max));
                                    total_heap.push(r, std::get<0>(r_max), std::get<1>(r_max));
                                }

                            } else {

                                total_heap.push(r, c, dq_vw);
                            }
                        };

                        update_elements(v, w);
                        update_elements(w, v);
                    }
                };

                update_delta_q_for_affected_commuinities();

                auto remove_u_entries = [&]() {
                    for (auto w : u_neighbours) {

                        delta_q.at(w).erase(u);

                        if (w != v) {

                            auto update_elements = [&](TQ r, TQ c) {
                                if (std::get<0>(delta_q_heaps.at(r).top()) == c) {

                                    delta_q_heaps.at(r).pop();
                                    total_heap.remove(r, c);

                                    auto [k, dq] = delta_q_heaps.at(r).top();

                                    if (delta_q_heaps.at(r).size() > 0) {
                                        total_heap.push(r, k, dq);
                                    }
                                } else {
                                    delta_q_heaps.at(r).remove(c);
                                }
                            };

                            update_elements(u, w);
                            update_elements(w, u);
                        }
                    }
                };

                remove_u_entries();

                delta_q.erase(u);
                delta_q_heaps.erase(u);
                a[v] += a[u];
                a[u] = 0;
            };

            merge_u_into_v();

            return top_delta_q;
        } else {

            return negative_infinity;
        }
    };

    while (communities.size() > cutoff) {

        auto dq = step();

        if (verbose) {
            display_communities(std::vector(communities), communities.size() > 10);
        }

        if (dq < 0) {

            break;
        }
    }

    Communities result;

    for (int i = 0; i < communities.size(); i++) {

        if (communities[i].size() > 0) {

            result.push_back(communities[i]);
        }
    }

    return result;
}

}

#endif