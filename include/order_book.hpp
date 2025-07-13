// include/order_book.hpp
#ifndef ORDER_BOOK_HPP
#define ORDER_BOOK_HPP

#include <string_view>
#include <map>
#include <optional>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include "depth_update.hpp"
#include "book_ticker_update.hpp"

/**
 * @brief OrderBook implementation for Binance streams
 * 
 * ASSUMPTIONS:
 * 1. Price levels with quantity 0 should be removed from the book
 * 2. Invalid price levels (bids >= best ask, asks <= best bid) are pruned automatically
 * 3. BookTicker updates overwrite existing levels at the same price
 * 4. DepthUpdate provides incremental updates to be merged with existing book
 * 5. Symbol validation is performed to ensure data consistency
 * 6. All prices and quantities are positive values
 */
class OrderBook {
public:
    explicit OrderBook(std::string_view symbol);

    void UpdateBookTicker(const BookTickerUpdate& data);
    void UpdateDepth(const DepthUpdate& data);
    
    std::optional<std::pair<std::pair<double, double>, std::pair<double, double>>> GetBestBidAsk() const;
    std::string ToString() const;
    void PruneMidBook();


private:
    std::string symbol_;
    std::map<double, double, std::greater<>> bids_;  // Price -> Quantity (descending order)
    std::map<double, double> asks_;                  // Price -> Quantity (ascending order)
};

#endif // ORDER_BOOK_HPP
