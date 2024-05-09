#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <fstream>

#include <ginv/clustering/clauset_newman_moore.hpp>
#include <ginv/clustering/decaying_max_heap.hpp>
#include <ginv/istanbul_ein_dataset.hpp>
#include <osigma/oconnections.hpp>
#include <osigma/ograph.hpp>
#include <osigma/onodes.hpp>


void create_communities(size_t node_count=0, std::string data_path = "./data/istanbul") {

    istanbul::IstanbulEinDatasetBin istanbul_dataset("./data/istanbul");
    std::cout << istanbul_dataset.describe() << std::endl;

    if (node_count > 0) {

        std::cout<<"Reducing the dataset"<<std::endl;

        istanbul_dataset.m_nodes.m_x_coordinates.resize(node_count);
        istanbul_dataset.m_nodes.m_y_coordinates.resize(node_count);
        istanbul_dataset.m_nodes.m_z_index.resize(node_count);
        
        std::apply([node_count](auto&... features){

            (features.resize(node_count), ...);
        }, istanbul_dataset.m_nodes.m_features);

        int i = 0;
        int subset_graph_connections_start = istanbul_dataset.connection_count();
        int subset_graph_connections_end = 0;

        while (istanbul_dataset.m_connections.m_from[i] < node_count) {

            if (
                (istanbul_dataset.m_connections.m_to[i] >= 0) &&
                (istanbul_dataset.m_connections.m_to[i] < node_count) &&
                (istanbul_dataset.m_connections.m_from[i] != istanbul_dataset.m_connections.m_to[i])
            ) {
                
                subset_graph_connections_start = std::min(subset_graph_connections_start, i);
                subset_graph_connections_end = std::max(subset_graph_connections_end, i);
            }

            i++;
        }

        auto erase_outside_range = []<typename T>(std::vector<T>& v, size_t start, size_t end) {

            if (start > 0) {

                v.erase(v.begin(), v.begin() + start);
            }
            
            if (end < v.size() - 1) {

                v.erase(v.begin() + end, v.end());
            }
        };
        
        erase_outside_range(istanbul_dataset.m_connections.m_from, subset_graph_connections_start, subset_graph_connections_end);
        erase_outside_range(istanbul_dataset.m_connections.m_to, subset_graph_connections_start, subset_graph_connections_end);
        erase_outside_range(istanbul_dataset.m_connections.m_values, subset_graph_connections_start, subset_graph_connections_end);
        erase_outside_range(istanbul_dataset.m_connections.m_z_index, subset_graph_connections_start, subset_graph_connections_end);

        std::cout << istanbul_dataset.describe() << std::endl;
    }

    auto communities = clustering::greedy_modularity_communities<float>(istanbul_dataset, 1.0f, 1, true);
    

    std::ofstream file("./communities_" + std::to_string(node_count) + ".txt");
    clustering::display_communities(communities, false, file);
    clustering::display_communities(communities, true, file);
    file.close();
}

int main(int argc, char** argv)
{

    create_communities(1000);

    return 0;
}