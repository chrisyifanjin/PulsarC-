#include "order_book.hpp"
#include <sstream>

OrderBook::OrderBook(std::string_view symbol) : symbol_(symbol) {}

void OrderBook::UpdateBookTicker(const BookTickerUpdate& data) {
    if (data.symbol != symbol_) {
        throw std::invalid_argument("Symbol mismatch in UpdateBookTicker");
    }

    // Update best bid
    if (data.bestBidPrice > 0 && data.bestBidQty > 0) {
        bids_[data.bestBidPrice] = data.bestBidQty;
    } else {
        bids_.erase(data.bestBidPrice);
    }

    // Update best ask
    if (data.bestAskPrice > 0 && data.bestAskQty > 0)
    {
        asks_[data.bestAskPrice] = data.bestAskQty;
    } else {
        asks_.erase(data.bestAskPrice);
    }

    PruneMidBook();
}

void OrderBook::UpdateDepth(const DepthUpdate& data) {
    for (const auto& [price, qty] : data.bids) {
        if (price > 0 && qty > 0)
            bids_[price] = qty;
        else
            bids_.erase(price);
    }

    for (const auto& [price, qty] : data.asks) {
        if (price > 0 && qty > 0)
            asks_[price] = qty;
        else
            asks_.erase(price);
    }

    PruneMidBook();
}

std::optional<std::pair<std::pair<double, double>, std::pair<double, double>>> OrderBook::GetBestBidAsk() const {
    if (bids_.empty() || asks_.empty()) return std::nullopt;
    return std::make_pair(*bids_.begin(), *asks_.begin());
}

std::string OrderBook::ToString() const {
    std::ostringstream oss;

    auto bidIt = bids_.begin();
    auto askIt = asks_.begin();
    for (int i = 1; i <= 20; ++i) {  // Start from 1 to match requirement format
        oss << "[" << std::setw(2) << i << "] ";

        if (bidIt != bids_.end()) {
            oss << "[ " << std::fixed << std::setprecision(5) << bidIt->second << " ] ";
            oss << std::fixed << std::setprecision(3) << bidIt->first;  // Match 3 decimal places
        } else {
            oss << "[ ------- ] ---------";
        }

        oss << " | ";

        if (askIt != asks_.end()) {
            oss << std::fixed << std::setprecision(3) << askIt->first << " [ ";  // Match 3 decimal places  
            oss << std::fixed << std::setprecision(5) << askIt->second << " ]";
        } else {
            oss << "--------- [ ------- ]";
        }

        oss << "\n";

        if (bidIt != bids_.end()) ++bidIt;
        if (askIt != asks_.end()) ++askIt;
    }
    return oss.str();
}


void OrderBook::PruneMidBook() {
    if (bids_.empty() || asks_.empty()) return;
    double bestBid = bids_.begin()->first;
    double bestAsk = asks_.begin()->first;

    // Erase bids with price >= bestAsk
    for (auto it = bids_.begin(); it != bids_.end();) {
        if (it->first >= bestAsk)
            it = bids_.erase(it);
        else
            ++it;
    }

    // Erase asks with price <= bestBid
    for (auto it = asks_.begin(); it != asks_.end();) {
        if (it->first <= bestBid)
            it = asks_.erase(it);
        else
            ++it;
    }
}

