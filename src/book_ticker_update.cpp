#include "book_ticker_update.hpp"

BookTickerUpdate BookTickerUpdate::FromJson(const std::string& json_str) {
    BookTickerUpdate update;
    auto j = nlohmann::json::parse(json_str);

    try {
        update.updateId = j.at("u").get<uint64_t>();
        update.symbol = j.at("s").get<std::string>();
        update.bestBidPrice = std::stod(j.at("b").get<std::string>());
        update.bestBidQty = std::stod(j.at("B").get<std::string>());
        update.bestAskPrice = std::stod(j.at("a").get<std::string>());
        update.bestAskQty = std::stod(j.at("A").get<std::string>());
    } catch (const nlohmann::json::exception& e) {
        throw std::runtime_error("Failed to parse BookTickerUpdate JSON: " + std::string(e.what()));
    }
    return update;
}