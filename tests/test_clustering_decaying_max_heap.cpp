#include <ginv/clustering/decaying_max_heap.hpp>
#include <gtest/gtest.h>
#include <tuple>
#include <vector>
#include <iostream>

TEST(ClusteringDecayingMaxHeap, MaxSortedPopsSingleKey)
{

    clustering::DecayingMaxHeap<float, uint8_t> heap(10);

    auto values = std::vector {
        std::make_tuple(14, 4.0f),
        std::make_tuple(11, 1.0f),
        std::make_tuple(13, 3.0f),
        std::make_tuple(10, 0.0f),
        std::make_tuple(12, 2.0f),
    };

    auto target = std::vector {
        std::make_tuple(14, 4.0f),
        std::make_tuple(13, 3.0f),
        std::make_tuple(12, 2.0f),
        std::make_tuple(11, 1.0f),
        std::make_tuple(10, 0.0f),
    };

    for (int i = 0; i < values.size(); i++) {

        heap.push(values[i]);
    }

    std::vector<std::tuple<uint8_t, float>> result;

    while (heap.size() > 0) {

        result.push_back(heap.pop());
    }

    ASSERT_EQ(target.size(), result.size());

    for (int i = 0; i < result.size(); i++) {

        EXPECT_EQ(target[i], result[i]);
    }
}

TEST(ClusteringDecayingMaxHeap, MaxSortedPopsMultipleKeys)
{

    clustering::DecayingMaxHeap<float, uint8_t, uint8_t, int32_t> heap(10);

    auto values = std::vector {
        std::make_tuple(14, 24, 100004, 4.0f),
        std::make_tuple(11, 21, 100001, 1.0f),
        std::make_tuple(13, 23, 100003, 3.0f),
        std::make_tuple(10, 20, 100000, 0.0f),
        std::make_tuple(12, 22, 100002, 2.0f),
    };

    auto target = std::vector {
        std::make_tuple(14, 24, 100004, 4.0f),
        std::make_tuple(13, 23, 100003, 3.0f),
        std::make_tuple(12, 22, 100002, 2.0f),
        std::make_tuple(11, 21, 100001, 1.0f),
        std::make_tuple(10, 20, 100000, 0.0f),
    };

    for (int i = 0; i < values.size(); i++) {

        heap.push(values[i]);
    }

    std::vector<std::tuple<uint8_t, uint8_t, int32_t, float>> result;

    while (heap.size() > 0) {

        result.push_back(heap.pop());
    }

    ASSERT_EQ(target.size(), result.size());

    for (int i = 0; i < result.size(); i++) {

        EXPECT_EQ(target[i], result[i]);
    }
}


TEST(ClusteringDecayingMaxHeap, ThrowsOnRemovingNonExistingElement)
{

    clustering::DecayingMaxHeap<float, uint8_t> heap(10);

    auto values = std::vector {
        std::make_tuple(14, 4.0f),
        std::make_tuple(11, 1.0f),
        std::make_tuple(13, 3.0f),
        std::make_tuple(10, 0.0f),
        std::make_tuple(12, 2.0f),
    };

    auto target = std::vector {
        std::make_tuple(14, 4.0f),
        std::make_tuple(13, 3.0f),
        std::make_tuple(12, 2.0f),
        std::make_tuple(11, 1.0f),
        std::make_tuple(10, 0.0f),
    };

    auto orders = std::vector{
        std::vector{113, 10, 11, 12, 14},
        std::vector{13, 210, 11, 14, 12},
        std::vector{11, 13, 1, 12, 14},
        std::vector{13, 14, 10, 311, 12},
        std::vector{11, 13, 10, 12, 124},
        std::vector{12, 13, 10, 11, -14},
    };

    for (int q = 0; q < orders.size(); q++) {

        for (int i = 0; i < values.size(); i++) {

            heap.push(values[i]);
        }

        EXPECT_THROW({

            for (int i = 0; i < orders[q].size(); i++) {

                heap.remove(orders[q][i]);
            }
        }, std::out_of_range);
        
        while (heap.size() > 0) {

            heap.pop();
        }
    }
}

TEST(ClusteringDecayingMaxHeap, UpdatesValues)
{

    clustering::DecayingMaxHeap<float, uint8_t> heap(10);

    auto values = std::vector {
        std::make_tuple(14, 4.0f),
        std::make_tuple(11, 1.0f),
        std::make_tuple(13, 3.0f),
        std::make_tuple(10, 0.0f),
        std::make_tuple(12, 2.0f),
    };

    auto target = std::vector {
        std::make_tuple(11, 4.0f),
        std::make_tuple(13, 3.0f),
        std::make_tuple(12, 2.0f),
        std::make_tuple(14, 1.0f),
        std::make_tuple(10, 0.0f),
    };

    for (int i = 0; i < values.size(); i++) {

        heap.push(values[i]);
    }

    heap.update_value(14, 1.0f);
    heap.update_value(11, 4.0f);

    std::vector<std::tuple<uint8_t, float>> result;

    while (heap.size() > 0) {

        result.push_back(heap.pop());
    }

    ASSERT_EQ(target.size(), result.size());

    for (int i = 0; i < result.size(); i++) {

        EXPECT_EQ(target[i], result[i]);
    }
}
