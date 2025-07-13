#include <depth_update.hpp>

DepthUpdate DepthUpdate::FromJson(const std::string& json_str) {
    DepthUpdate update;

    try {
        auto j = nlohmann::json::parse(json_str);
        update.lastUpdateId = j.at("lastUpdateId").get<uint64_t>();

        for (const auto& bid : j.at("bids")) {
            if (bid.size() != 2) continue;
            double price = std::stod(bid[0].get<std::string>());
            double qty   = std::stod(bid[1].get<std::string>());
            update.bids.emplace_back(price, qty);
        }

        for (const auto& ask : j.at("asks")) {
            if (ask.size() != 2) continue;
            double price = std::stod(ask[0].get<std::string>());
            double qty   = std::stod(ask[1].get<std::string>());
            update.asks.emplace_back(price, qty);
        }

    } catch (const std::exception& e) {
        throw std::invalid_argument("Invalid DepthUpdate JSON: " + std::string(e.what()));
    }

    return update;
}
