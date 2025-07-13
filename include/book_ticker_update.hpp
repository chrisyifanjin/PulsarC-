// include/book_ticker_update.hpp
#ifndef BOOK_TICKER_UPDATE_HPP
#define BOOK_TICKER_UPDATE_HPP

#include <cstdint>
#include <string>
#include "nlonmann/json.hpp"

struct BookTickerUpdate {
    uint64_t updateId;
    std::string symbol;
    double bestBidPrice;
    double bestBidQty;
    double bestAskPrice;
    double bestAskQty;

    static BookTickerUpdate FromJson(const std::string& json_str);
};

#endif // BOOK_TICKER_UPDATE_HPP