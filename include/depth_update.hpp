// include/depth_update.hpp
#ifndef DEPTH_UPDATE_HPP
#define DEPTH_UPDATE_HPP

#include <cstdint>
#include <vector>
#include <string>
#include "nlonmann/json.hpp"
#include <stdexcept>

struct DepthUpdate {
    uint64_t lastUpdateId;
    std::vector<std::pair<double, double>> bids;
    std::vector<std::pair<double, double>> asks;

    static DepthUpdate FromJson(const std::string& json_str);
};

#endif // DEPTH_UPDATE_HPP