#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <tuple>
#include <utility>

#include <cstdarg>

#include <ginv/istanbul_ein_dataset.hpp>
#include <osigma/oconnections.hpp>
#include <osigma/onodes.hpp>

using istanbul::IstanbulEinDatasetBin;
using json = nlohmann::json;

template <typename T>
void read(std::string file_name, int file_count, std::vector<T> storage)
{

    size_t offset = 0;
    std::string to_replace = "${FILE_ID}";

    for (int i = 0; i < file_count; i++) {

        std::string current_file_name(file_name);
        current_file_name.replace(current_file_name.find(to_replace), to_replace.size(), std::to_string(i));

        std::ifstream file(current_file_name, std::ios::in | std::ios::binary);

        file.unsetf(std::ios::skipws);
        file.seekg(0, std::ios::end);
        size_t file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        size_t buffer_size = file_size / sizeof(T);

        file.read((char*)&storage[offset], file_size);

        std::cout << file_size << " " << +storage[offset] << " " << +storage[offset + 1] << " " << +storage[offset + 2] << " " << +storage[offset + 3] << std::endl;

        offset += buffer_size;
        std::cout << "loaded " << current_file_name << std::endl;
    }
}

istanbul::IstanbulEinDatasetBin::IstanbulEinDatasetBin(std::string root, std::string global_params_file)
    : OGraph<int32_t, uint8_t, float, uint8_t,
        int32_t, float, int32_t, float, float, float>(
        ograph::OSpatialNodes<float, uint8_t, int32_t, float, int32_t, float, float, float>(
            std::vector<float>(), std::vector<float>(), std::vector<uint8_t>(), std::vector<int32_t>(), std::vector<float>(), std::vector<int32_t>(),
            std::vector<float>(), std::vector<float>(), std::vector<float>()),
        ograph::OSpatialConnections<int32_t, uint8_t, uint8_t>(std::vector<int32_t>(), std::vector<int32_t>(), std::vector<uint8_t>(), std::vector<uint8_t>()))
{

    load_dataset(root, global_params_file);
}
std::string istanbul::IstanbulEinDatasetBin::describe() const
{
    return "IstanbulEinDatasetBin(with " + m_nodes.describe() + " and " + m_connections.describe() + ")";
}
void istanbul::IstanbulEinDatasetBin::load_dataset(std::string root, std::string global_params_file)
{
    std::ifstream f(root + "/" + global_params_file);
    json global_params_json = json::parse(f);
    int nodes = global_params_json["nodes"];
    int connections = global_params_json["links"];

    m_nodes.m_x_coordinates.resize(nodes);
    m_nodes.m_y_coordinates.resize(nodes);

    std::apply([&nodes](auto&&... features) { ((features.resize(nodes)), ...); }, m_nodes.m_features);

    m_connections.m_from.resize(connections);
    m_connections.m_to.resize(connections);
    m_connections.m_values.resize(connections);
    m_connections.m_z_index.resize(connections);

    std::apply([&connections](auto&&... features) { ((features.resize(connections)), ...); }, m_connections.m_features);

    load_ein_bins(root);
    load_features(root);

    return;
}
void istanbul::IstanbulEinDatasetBin::load_ein_bins(
    std::string ein_folder, std::string from_name, std::string to_name,
    std::string value_name, std::tuple<int, int, int> file_counts)
{

    read(ein_folder + "/" + from_name, std::get<0>(file_counts), m_connections.m_from);
    read(ein_folder + "/" + to_name, std::get<1>(file_counts), m_connections.m_to);
    read(ein_folder + "/" + value_name, std::get<2>(file_counts), m_connections.m_values);
}

void istanbul::IstanbulEinDatasetBin::load_features(std::string root)
{
    read(root + "/" + std::get<0>(m_feature_files), m_feature_file_count, std::get<0>(m_nodes.m_features));
    read(root + "/" + std::get<1>(m_feature_files), m_feature_file_count, std::get<1>(m_nodes.m_features));
    read(root + "/" + std::get<2>(m_feature_files), m_feature_file_count, std::get<2>(m_nodes.m_features));
    read(root + "/" + std::get<3>(m_feature_files), m_feature_file_count, std::get<3>(m_nodes.m_features));
    read(root + "/" + std::get<4>(m_feature_files), m_feature_file_count, std::get<4>(m_nodes.m_features));
    read(root + "/" + std::get<5>(m_feature_files), m_feature_file_count, std::get<5>(m_nodes.m_features));
}