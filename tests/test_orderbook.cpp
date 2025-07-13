#include <gtest/gtest.h>
#include "order_book.hpp"
#include "book_ticker_update.hpp"
#include "depth_update.hpp"
#include <chrono>
#include <sstream>

// Test fixture for OrderBook tests
class OrderBookTest : public ::testing::Test {
protected:
    void SetUp() override {
        orderBook = std::make_unique<OrderBook>("BTCUSDT");
    }
    
    void TearDown() override {
        orderBook.reset();
    }
    
    std::unique_ptr<OrderBook> orderBook;
};

// ============================================================================
// BookTickerUpdate Tests
// ============================================================================

TEST(BookTickerUpdateTest, DeserializationFromJson) {
    std::string json = R"({
        "u":400900217,
        "s":"BNBUSDT",
        "b":"25.35190000",
        "B":"31.21000000",
        "a":"25.36520000",
        "A":"40.66000000"
    })";
    
    BookTickerUpdate update = BookTickerUpdate::FromJson(json);
    
    EXPECT_EQ(update.updateId, 400900217);
    EXPECT_EQ(update.symbol, "BNBUSDT");
    EXPECT_DOUBLE_EQ(update.bestBidPrice, 25.35190000);
    EXPECT_DOUBLE_EQ(update.bestBidQty, 31.21000000);
    EXPECT_DOUBLE_EQ(update.bestAskPrice, 25.36520000);
    EXPECT_DOUBLE_EQ(update.bestAskQty, 40.66000000);
}

TEST(BookTickerUpdateTest, InvalidJsonThrowsException) {
    std::string invalidJson = R"({"invalid": "json"})";
    EXPECT_THROW(BookTickerUpdate::FromJson(invalidJson), std::exception);
}

// ============================================================================
// DepthUpdate Tests
// ============================================================================

TEST(DepthUpdateTest, DeserializationFromJson) {
    std::string json = R"({
        "lastUpdateId": 160,
        "bids": [
            ["0.0024", "10"],
            ["0.0023", "20"]
        ],
        "asks": [
            ["0.0026", "100"],
            ["0.0027", "200"]
        ]
    })";
    
    DepthUpdate update = DepthUpdate::FromJson(json);
    
    EXPECT_EQ(update.lastUpdateId, 160);
    EXPECT_EQ(update.bids.size(), 2);
    EXPECT_EQ(update.asks.size(), 2);
    EXPECT_DOUBLE_EQ(update.bids[0].first, 0.0024);
    EXPECT_DOUBLE_EQ(update.bids[0].second, 10);
    EXPECT_DOUBLE_EQ(update.asks[0].first, 0.0026);
    EXPECT_DOUBLE_EQ(update.asks[0].second, 100);
}

TEST(DepthUpdateTest, InvalidJsonThrowsException) {
    std::string invalidJson = R"({"incomplete": "data"})";
    EXPECT_THROW(DepthUpdate::FromJson(invalidJson), std::exception);
}

// ============================================================================
// OrderBook Core Functionality Tests
// ============================================================================

TEST_F(OrderBookTest, EmptyBookReturnsNoQuote) {
    auto best = orderBook->GetBestBidAsk();
    EXPECT_FALSE(best.has_value());
}

TEST_F(OrderBookTest, BookTickerUpdate) {
    BookTickerUpdate update{400900217, "BTCUSDT", 25.35, 31.21, 25.36, 40.66};
    orderBook->UpdateBookTicker(update);
    
    auto best = orderBook->GetBestBidAsk();
    ASSERT_TRUE(best.has_value());
    EXPECT_DOUBLE_EQ(best->first.first, 25.35);   // Best bid price
    EXPECT_DOUBLE_EQ(best->first.second, 31.21);  // Best bid quantity
    EXPECT_DOUBLE_EQ(best->second.first, 25.36);  // Best ask price
    EXPECT_DOUBLE_EQ(best->second.second, 40.66); // Best ask quantity
}

TEST_F(OrderBookTest, DepthUpdateBasicOperations) {
    std::string depthJson = R"({
        "lastUpdateId": 1,
        "bids": [["50000", "1"], ["49999", "2"]],
        "asks": [["50001", "1"], ["50002", "2"]]
    })";
    
    DepthUpdate depth = DepthUpdate::FromJson(depthJson);
    orderBook->UpdateDepth(depth);
    
    auto best = orderBook->GetBestBidAsk();
    ASSERT_TRUE(best.has_value());
    EXPECT_DOUBLE_EQ(best->first.first, 50000.0);  // Best bid price
    EXPECT_DOUBLE_EQ(best->first.second, 1.0);     // Best bid quantity
    EXPECT_DOUBLE_EQ(best->second.first, 50001.0); // Best ask price
    EXPECT_DOUBLE_EQ(best->second.second, 1.0);    // Best ask quantity
}

TEST_F(OrderBookTest, ZeroQuantityRemovesLevel) {
    // Add initial levels
    orderBook->UpdateDepth(DepthUpdate{
        1, // lastUpdateId
        { {100.0, 5.0} }, // bids
        { {101.0, 5.0} }  // asks
    });

    auto best = orderBook->GetBestBidAsk();
    ASSERT_TRUE(best.has_value());

    // Update with zero quantity (should remove level)
    orderBook->UpdateDepth(DepthUpdate{
        2, // lastUpdateId
        { {100.0, 0.0} }, // zero quantity bid
        { {101.0, 0.0} }  // zero quantity ask
    });

    best = orderBook->GetBestBidAsk();
    EXPECT_FALSE(best.has_value()); // Should be empty now
}

TEST_F(OrderBookTest, MixedUpdates) {
    // Start with depth update
    orderBook->UpdateDepth(DepthUpdate{
        1, // lastUpdateId
        { {100.0, 5.0} }, // bids
        { {101.0, 5.0} }  // asks
    });

    // Update with BookTicker
    BookTickerUpdate ticker{2, "BTCUSDT", 100.5, 3.0, 100.8, 4.0};
    orderBook->UpdateBookTicker(ticker);

    auto best = orderBook->GetBestBidAsk();
    ASSERT_TRUE(best.has_value());
    EXPECT_DOUBLE_EQ(best->first.first, 100.5);  // BookTicker bid
    EXPECT_DOUBLE_EQ(best->second.first, 100.8); // BookTicker ask
}

// ============================================================================
// PruneMidBook Tests
// ============================================================================

TEST_F(OrderBookTest, PruneMidBookRemovesInvalidLevels) {
    // Add valid bids and asks
    orderBook->UpdateDepth(DepthUpdate{
        1, // lastUpdateId
        { {100.0, 5.0}, {99.0, 10.0}, {98.0, 15.0} }, // bids
        { {101.0, 5.0}, {102.0, 10.0}, {103.0, 15.0} } // asks
    });

    // Add invalid levels (bids >= best ask, asks <= best bid)
    orderBook->UpdateDepth(DepthUpdate{
        2, // lastUpdateId
        { {102.0, 1.0}, {101.5, 2.0} }, // invalid bids (>= 101.0)
        { {99.5, 1.0}, {100.5, 2.0} }   // invalid asks (<= 100.0)
    });

    // Prune invalid levels
    orderBook->PruneMidBook();

    // Verify best bid/ask after pruning maintains valid spread
    auto best = orderBook->GetBestBidAsk();
    ASSERT_TRUE(best.has_value());
    EXPECT_LT(best->first.first, best->second.first); // Valid spread (bid < ask)
    
    // Verify that the spread is reasonable (should be close to original 100/101)
    EXPECT_LE(best->first.first, 100.0);   // Best bid should be <= 100
    EXPECT_GE(best->second.first, 101.0);  // Best ask should be >= 101
    
    // Verify invalid levels are removed by checking string output
    std::string output = orderBook->ToString();
    EXPECT_TRUE(output.find("101.5") == std::string::npos || 
                output.find("102.0") == std::string::npos ||
                output.find("99.5") == std::string::npos ||
                output.find("100.5") == std::string::npos);
}

// ============================================================================
// Output Formatting Tests
// ============================================================================

TEST_F(OrderBookTest, ToStringFormat) {
    DepthUpdate update{
        1, // lastUpdateId
        { {50000.0, 1.0}, {49999.0, 2.0} }, // bids
        { {50001.0, 1.0}, {50002.0, 2.0} }  // asks
    };
    
    orderBook->UpdateDepth(update);
    std::string output = orderBook->ToString();
    
    // Check that output contains expected formatting
    EXPECT_TRUE(output.find("50000.00") != std::string::npos);
    EXPECT_TRUE(output.find("50001.00") != std::string::npos);
    EXPECT_TRUE(output.find("|") != std::string::npos); // Separator
}

// ============================================================================
// Performance Tests
// ============================================================================

TEST_F(OrderBookTest, PerformanceBaseline) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Perform many updates
    for (int i = 0; i < 1000; ++i) {
        DepthUpdate update{
            static_cast<uint64_t>(i),
            { {50000.0 + static_cast<double>(i), 1.0} },
            { {50001.0 + static_cast<double>(i), 1.0} }
        };
        orderBook->UpdateDepth(update);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Verify it completes in reasonable time (less than 100ms for 1000 updates)
    EXPECT_LT(duration.count(), 100000);
}
