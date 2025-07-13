#include "order_book.hpp"
#include <iostream>

int main() {
    // Create an order book for BNBUSDT
    OrderBook book("BNBUSDT");

    // Sample depth update JSON (Partial Book Depth)
    std::string depthJson = R"(
    {
        "lastUpdateId": 160,
        "bids": [
            ["25.00", "5"],
            ["24.50", "2"]
        ],
        "asks": [
            ["25.50", "3"],
            ["26.00", "7"]
        ]
    })";

    // Sample book ticker update JSON (best bid/ask)
    std::string tickerJson = R"(
    {
        "u":400900217,
        "s":"BNBUSDT",
        "b":"24.60",
        "B":"4",
        "a":"25.40",
        "A":"6"
    })";

    try {
        DepthUpdate depth = DepthUpdate::FromJson(depthJson);
        BookTickerUpdate ticker = BookTickerUpdate::FromJson(tickerJson);

        book.UpdateDepth(depth);
        book.UpdateBookTicker(ticker);

        std::cout << "=== Order Book ===\n";
        std::cout << book.ToString();

        auto best = book.GetBestBidAsk();
        if (best) {
            std::cout << "\nBest Bid: " << best->first.first << " Qty: " << best->first.second << "\n";
            std::cout << "Best Ask: " << best->second.first << " Qty: " << best->second.second << "\n";
        } else {
            std::cout << "\nOrder book is empty or invalid.\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}
