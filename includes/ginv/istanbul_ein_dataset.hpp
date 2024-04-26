#ifndef ISTANBUL_EIN_DATASET_HPP_
#define ISTANBUL_EIN_DATASET_HPP_

#include <osigma/ograph.hpp>
#include <string>

namespace istanbul {

class IstanbulEinDatasetBin : public ograph::OGraph<
                                  int32_t, uint8_t, float, uint8_t,
                                  int32_t, float, int32_t, float, float, float> {

public:
    explicit IstanbulEinDatasetBin(std::string root, std::string global_params_file = "global_params.json");
    std::string describe() const;

private:
    const int m_feature_count = 6;
    const int m_feature_file_count = 1;
    std::tuple<std::string, std::string, std::string, std::string, std::string, std::string> m_feature_files = std::make_tuple(
        "feature_degree_${FILE_ID}.bin",
        "feature_centrality_${FILE_ID}.bin",
        "feature_number_of_trades_${FILE_ID}.bin",
        "feature_profits_${FILE_ID}.bin",
        "feature_profits_excess_${FILE_ID}.bin",
        "feature_volume_${FILE_ID}.bin");

    void load_dataset(std::string root, std::string global_params_file);
    void load_features(std::string root);
    void load_ein_bins(
        std::string ein_folder, std::string from_name = "ein_from_${FILE_ID}.bin", std::string to_name = "ein_to_${FILE_ID}.bin",
        std::string value_name = "ein_value_${FILE_ID}.bin", std::tuple<int, int, int> file_counts = std::make_tuple(2, 2, 1));
};
}

#endif