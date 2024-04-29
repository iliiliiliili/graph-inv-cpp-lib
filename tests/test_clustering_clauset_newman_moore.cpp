#include <ginv/clustering/clauset_newman_moore.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <tuple>
#include <vector>

TEST(ClusteringClausetNewmanMoore, CreatesNoCommunitiesOfDisconnectedGraph)
{

    ograph::OGraph<int32_t, float, float, uint8_t, float> g(
        ograph::OSpatialNodes<float, uint8_t, float>(
            std::vector<float>(5),
            std::vector<float>(5),
            std::vector<uint8_t>(5),
            std::vector<float>(5)),
        ograph::OSpatialConnections<int32_t, float, uint8_t>(
            std::vector<int32_t>(0),
            std::vector<int32_t>(0),
            std::vector<float>(0),
            std::vector<uint8_t>(0)));

    auto target = std::vector {
        std::vector { 0 },
        std::vector { 1 },
        std::vector { 2 },
        std::vector { 3 },
        std::vector { 4 },
    };

    auto communities = clustering::greedy_modularity_communities<float>(g);

    EXPECT_EQ(target, communities);
}

TEST(ClusteringClausetNewmanMoore, CreatesSingleCommunityOfGraphWithOneLink)
{

    ograph::OGraph<int32_t, float, float, uint8_t, float> g(
        ograph::OSpatialNodes<float, uint8_t, float>(
            std::vector<float>(5),
            std::vector<float>(5),
            std::vector<uint8_t>(5),
            std::vector<float>(5)),
        ograph::OSpatialConnections<int32_t, float, uint8_t>(
            std::vector<int32_t> { 2 },
            std::vector<int32_t> { 3 },
            std::vector<float> { 1 },
            std::vector<uint8_t> { 0 }));

    auto target = std::vector {
        std::vector { 0 },
        std::vector { 1 },
        std::vector { 3, 2 },
        std::vector { 4 },
    };

    auto communities = clustering::greedy_modularity_communities<float>(g);

    EXPECT_EQ(target, communities);
}

TEST(ClusteringClausetNewmanMoore, CreatesTwoParallelCommunitiesOfGraphWIthTwoGroupsOfLinkedNodes)
{

    ograph::OGraph<int32_t, float, float, uint8_t, float> g(
        ograph::OSpatialNodes<float, uint8_t, float>(
            std::vector<float>(5),
            std::vector<float>(5),
            std::vector<uint8_t>(5),
            std::vector<float>(5)),
        ograph::OSpatialConnections<int32_t, float, uint8_t>(
            std::vector<int32_t> { 2, 2, 1 },
            std::vector<int32_t> { 3, 0, 4 },
            std::vector<float> { 1, 1, 1 },
            std::vector<uint8_t> { 0, 0, 0 }));

    auto target = std::vector {
        std::vector { 3, 0, 2 },
        std::vector { 4, 1 },
    };

    auto communities = clustering::greedy_modularity_communities<float>(g);

    EXPECT_EQ(target, communities);
}

TEST(ClusteringClausetNewmanMoore, DoesNotCombineAllNodesOfChainGraph)
{

    ograph::OGraph<int32_t, float, float, uint8_t, float> g(
        ograph::OSpatialNodes<float, uint8_t, float>(
            std::vector<float>(5),
            std::vector<float>(5),
            std::vector<uint8_t>(5),
            std::vector<float>(5)),
        ograph::OSpatialConnections<int32_t, float, uint8_t>(
            std::vector<int32_t> { 0, 1, 2, 3 },
            std::vector<int32_t> { 1, 2, 3, 4 },
            std::vector<float> { 1, 1, 1, 1 },
            std::vector<uint8_t> { 0, 0, 0, 0 }));

    auto target = std::vector {
        std::vector { 2, 1, 0 },
        std::vector { 4, 3 },
    };

    auto communities = clustering::greedy_modularity_communities<float>(g);

    EXPECT_EQ(target, communities);
}

TEST(ClusteringClausetNewmanMoore, DoesNotCombineAllNodesOfCircleGraph)
{

    ograph::OGraph<int32_t, float, float, uint8_t, float> g(
        ograph::OSpatialNodes<float, uint8_t, float>(
            std::vector<float>(5),
            std::vector<float>(5),
            std::vector<uint8_t>(5),
            std::vector<float>(5)),
        ograph::OSpatialConnections<int32_t, float, uint8_t>(
            std::vector<int32_t> { 0, 1, 2, 3, 4 },
            std::vector<int32_t> { 1, 2, 3, 4, 0 },
            std::vector<float> { 1, 1, 1, 1, 1 },
            std::vector<uint8_t> { 0, 0, 0, 0, 0 }));

    auto target = std::vector {
        std::vector { 1, 0 },
        std::vector { 4, 2, 3 },
    };

    auto communities = clustering::greedy_modularity_communities<float>(g);

    EXPECT_EQ(target, communities);
}

TEST(ClusteringClausetNewmanMoore, CombinesAllNodesOfFullGraph)
{
    int node_count = 26;
    int connection_count = node_count * (node_count - 1) / 2;

    ograph::OGraph<int32_t, float, float, uint8_t, float> g(
        ograph::OSpatialNodes<float, uint8_t, float>(
            std::vector<float>(0 + node_count),
            std::vector<float>(node_count),
            std::vector<uint8_t>(node_count),
            std::vector<float>(node_count)),
        ograph::OSpatialConnections<int32_t, float, uint8_t>(
            std::vector<int32_t>(connection_count),
            std::vector<int32_t>(connection_count),
            std::vector<float>(connection_count, 1),
            std::vector<uint8_t>(connection_count)));

    auto target = std::vector {
        std::vector { 1, 0 },
        std::vector { 4, 2, 3 },
    };

    int t = 0;

    for (int i = 0; i < node_count; i++) {
        for (int j = 0; j < i; j++) {
            g.m_connections.m_from[t] = i;
            g.m_connections.m_to[t] = j;

            t++;
        }
    }

    auto communities = clustering::greedy_modularity_communities<float>(g);

    EXPECT_EQ(1, communities.size());
}

TEST(ClusteringClausetNewmanMoore, CombinesAllNodesOfTwoFullGraphsInParallel)
{
    int node_count_a = 26;
    int node_count_b = 5;
    int connection_count_a = node_count_a * (node_count_a - 1) / 2;
    int connection_count_b = node_count_b * (node_count_b - 1) / 2;
    int node_count = node_count_a + node_count_b;
    int connection_count = connection_count_a + connection_count_b;

    ograph::OGraph<int32_t, float, float, uint8_t, float> g(
        ograph::OSpatialNodes<float, uint8_t, float>(
            std::vector<float>(0 + node_count),
            std::vector<float>(node_count),
            std::vector<uint8_t>(node_count),
            std::vector<float>(node_count)),
        ograph::OSpatialConnections<int32_t, float, uint8_t>(
            std::vector<int32_t>(connection_count),
            std::vector<int32_t>(connection_count),
            std::vector<float>(connection_count, 1),
            std::vector<uint8_t>(connection_count)));

    auto target = std::vector {
        std::vector { 1, 0 },
        std::vector { 4, 2, 3 },
    };

    int t = 0;

    for (int i = 0; i < node_count_a; i++) {
        for (int j = 0; j < i; j++) {
            g.m_connections.m_from[t] = i;
            g.m_connections.m_to[t] = j;

            t++;
        }
    }

    for (int i = 0; i < node_count_b; i++) {
        for (int j = 0; j < i; j++) {
            g.m_connections.m_from[t] = i + node_count_a;
            g.m_connections.m_to[t] = j + node_count_a;

            t++;
        }
    }

    auto communities = clustering::greedy_modularity_communities<float>(g);

    EXPECT_EQ(2, communities.size());
}

TEST(ClusteringClausetNewmanMoore, DoesNotCombineTwoFullGraphsWithSingleLinkBetweenThem)
{
    int node_count_a = 26;
    int node_count_b = 5;
    int connection_count_a = node_count_a * (node_count_a - 1) / 2;
    int connection_count_b = node_count_b * (node_count_b - 1) / 2;
    int node_count = node_count_a + node_count_b;
    int connection_count = connection_count_a + connection_count_b + 1;

    ograph::OGraph<int32_t, float, float, uint8_t, float> g(
        ograph::OSpatialNodes<float, uint8_t, float>(
            std::vector<float>(0 + node_count),
            std::vector<float>(node_count),
            std::vector<uint8_t>(node_count),
            std::vector<float>(node_count)),
        ograph::OSpatialConnections<int32_t, float, uint8_t>(
            std::vector<int32_t>(connection_count),
            std::vector<int32_t>(connection_count),
            std::vector<float>(connection_count, 1),
            std::vector<uint8_t>(connection_count)));

    auto target = std::vector {
        std::vector { 1, 0 },
        std::vector { 4, 2, 3 },
    };

    int t = 0;

    for (int i = 0; i < node_count_a; i++) {
        for (int j = 0; j < i; j++) {
            g.m_connections.m_from[t] = i;
            g.m_connections.m_to[t] = j;

            t++;
        }
    }

    for (int i = 0; i < node_count_b; i++) {
        for (int j = 0; j < i; j++) {
            g.m_connections.m_from[t] = i + node_count_a;
            g.m_connections.m_to[t] = j + node_count_a;

            t++;
        }
    }

    g.m_connections.m_from[t] = 1 + node_count_a;
    g.m_connections.m_to[t] = 1;

    auto communities = clustering::greedy_modularity_communities<float>(g);

    EXPECT_EQ(2, communities.size());
}

TEST(ClusteringClausetNewmanMoore, CreatesNewSeparateCommunityOfTwoFullGraphsWithSingleHighValueLinkBetweenThem)
{
    int node_count_a = 26;
    int node_count_b = 5;
    int connection_count_a = node_count_a * (node_count_a - 1) / 2;
    int connection_count_b = node_count_b * (node_count_b - 1) / 2;
    int node_count = node_count_a + node_count_b;
    int connection_count = connection_count_a + connection_count_b + 1;

    ograph::OGraph<int32_t, float, float, uint8_t, float> g(
        ograph::OSpatialNodes<float, uint8_t, float>(
            std::vector<float>(0 + node_count),
            std::vector<float>(node_count),
            std::vector<uint8_t>(node_count),
            std::vector<float>(node_count)),
        ograph::OSpatialConnections<int32_t, float, uint8_t>(
            std::vector<int32_t>(connection_count),
            std::vector<int32_t>(connection_count),
            std::vector<float>(connection_count, 1),
            std::vector<uint8_t>(connection_count)));

    auto target = std::vector {
        std::vector { 1, 0 },
        std::vector { 4, 2, 3 },
    };

    int t = 0;

    for (int i = 0; i < node_count_a; i++) {
        for (int j = 0; j < i; j++) {
            g.m_connections.m_from[t] = i;
            g.m_connections.m_to[t] = j;

            t++;
        }
    }

    for (int i = 0; i < node_count_b; i++) {
        for (int j = 0; j < i; j++) {
            g.m_connections.m_from[t] = i + node_count_a;
            g.m_connections.m_to[t] = j + node_count_a;

            t++;
        }
    }

    g.m_connections.m_from[t] = 1 + node_count_a;
    g.m_connections.m_to[t] = 1;
    g.m_connections.m_values[t] = 255;

    auto target_extra_community = std::vector<int32_t> { 1 + node_count_a, 1 };

    auto communities = clustering::greedy_modularity_communities<float>(g);

    ASSERT_EQ(3, communities.size());
    EXPECT_EQ(target_extra_community, communities[1]);
}

TEST(ClusteringClausetNewmanMoore, CombinesAllNodesOfTwoBigFullGraphsInParallel)
{
    int node_count_a = 1000;
    int node_count_b = 800;
    int connection_count_a = node_count_a * (node_count_a - 1) / 2;
    int connection_count_b = node_count_b * (node_count_b - 1) / 2;
    int node_count = node_count_a + node_count_b;
    int connection_count = connection_count_a + connection_count_b;

    ograph::OGraph<int32_t, float, float, uint8_t, float> g(
        ograph::OSpatialNodes<float, uint8_t, float>(
            std::vector<float>(0 + node_count),
            std::vector<float>(node_count),
            std::vector<uint8_t>(node_count),
            std::vector<float>(node_count)),
        ograph::OSpatialConnections<int32_t, float, uint8_t>(
            std::vector<int32_t>(connection_count),
            std::vector<int32_t>(connection_count),
            std::vector<float>(connection_count, 1),
            std::vector<uint8_t>(connection_count)));

    auto target = std::vector {
        std::vector { 1, 0 },
        std::vector { 4, 2, 3 },
    };

    int t = 0;

    for (int i = 0; i < node_count_a; i++) {
        for (int j = 0; j < i; j++) {
            g.m_connections.m_from[t] = i;
            g.m_connections.m_to[t] = j;

            t++;
        }
    }

    for (int i = 0; i < node_count_b; i++) {
        for (int j = 0; j < i; j++) {
            g.m_connections.m_from[t] = i + node_count_a;
            g.m_connections.m_to[t] = j + node_count_a;

            t++;
        }
    }

    auto communities = clustering::greedy_modularity_communities<float>(g);

    EXPECT_EQ(2, communities.size());
}
