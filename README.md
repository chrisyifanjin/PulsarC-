# Binance Order Book Implementation

A high-performance C++ implementation of a Binance order book that processes Individual Symbol Book Ticker and Partial Book Depth streams with real-time updates.

## Features

- **Real-time Order Book Management**: Efficiently processes Binance WebSocket streams
- **JSON Message Processing**: Handles BookTicker and DepthUpdate messages  
- **High Performance**: Uses `std::map` with O(log n) operations for price-time priority
- **Robust Error Handling**: Comprehensive validation for malformed data
- **Modern C++17**: Leverages modern C++ features and best practices
- **Comprehensive Testing**: Full Google Test suite with performance benchmarks

## Project Structure

```
BinanceOrderBook/
├── CMakeLists.txt          # Build configuration with modern dependency management
├── README.md               # Project documentation  
├── include/                # Public headers
│   ├── order_book.hpp
│   ├── book_ticker_update.hpp
│   └── depth_update.hpp
├── src/                    # Implementation files
│   ├── main.cpp           # Demo application
│   ├── order_book.cpp
│   ├── book_ticker_update.cpp
│   └── depth_update.cpp
├── tests/                  # Test suite
│   └── test_orderbook.cpp # Comprehensive Google Test suite
└── third_party/           # External dependencies
    └── nlohmann/          # JSON library (header-only)
        └── json.hpp
```

## Dependencies

This project uses modern CMake dependency management:

- **Google Test**: Automatically downloaded via CMake FetchContent
- **nlohmann/json**: Header-only library included in `third_party/`

No manual dependency installation required!

## Requirements

- **C++17** compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake 3.15+**

## Quick Start

### Build the Project

```bash
# Create build directory and configure
mkdir build && cd build
cmake ..

# Build all targets
cmake --build . --parallel

# Or build specific targets
cmake --build . --target orderbook_demo
cmake --build . --target orderbook_tests
```

### Run Demo

```bash
# From build directory
./orderbook_demo
```

### Run Tests

```bash
# Run all tests
./orderbook_tests

# Or use CTest for detailed output
ctest --verbose
```

## API Usage

### Basic Order Book Operations

```cpp
#include "order_book.hpp"

// Create order book for a symbol
OrderBook book("BTCUSDT");

// Process depth update
std::string depthJson = R"({
    "lastUpdateId": 160,
    "bids": [["50000.00", "1.5"], ["49999.00", "2.0"]],
    "asks": [["50001.00", "1.0"], ["50002.00", "3.0"]]
})";

DepthUpdate depth = DepthUpdate::FromJson(depthJson);
book.UpdateDepth(depth);

// Process book ticker update  
BookTickerUpdate ticker{12345, "BTCUSDT", 50000.5, 1.2, 50001.5, 0.8};
book.UpdateBookTicker(ticker);

// Get best bid/ask
auto bestQuote = book.GetBestBidAsk();
if (bestQuote.has_value()) {
    auto [bid, ask] = *bestQuote;
    std::cout << "Best Bid: " << bid.first << " @ " << bid.second << std::endl;
    std::cout << "Best Ask: " << ask.first << " @ " << ask.second << std::endl;
}

// Display order book
std::cout << book.ToString() << std::endl;
```

### Message Processing

```cpp
// Parse JSON messages from Binance streams
std::string tickerJson = R"({
    "u": 400900217,
    "s": "BNBUSDT", 
    "b": "25.35190000",
    "B": "31.21000000",
    "a": "25.36520000", 
    "A": "40.66000000"
})";

BookTickerUpdate ticker = BookTickerUpdate::FromJson(tickerJson);
book.UpdateBookTicker(ticker);
```

## Performance

- **Update Latency**: Sub-microsecond for individual updates
- **Memory Efficient**: Automatic cleanup of zero-quantity levels
- **Scalable**: Handles high-frequency updates with logarithmic complexity

## Testing

The project includes comprehensive Google Tests covering:

- ✅ JSON deserialization and validation
- ✅ Order book state management  
- ✅ Best bid/ask calculations
- ✅ Price level updates and removals
- ✅ Invalid data handling
- ✅ Performance benchmarks
- ✅ Edge cases and error conditions

Run tests with: `./orderbook_tests`

## License

This project is provided as-is for educational and demonstration purposes.

# Configure and build
cmake ..
make -j$(nproc)

# Run demo
./orderbook_demo

# Run tests (simple)
./orderbook_tests

# Run Google Tests
./orderbook_gtest

# Run with CTest
ctest --verbose
```

### Build with Clang++ (Alternative)

```bash
# Build demo
clang++ -std=c++17 -O2 -I include src/main.cpp src/*.cpp -o orderbook_demo

# Build tests
clang++ -std=c++17 -O2 -I include src/test_main.cpp src/order_book.cpp src/depth_update.cpp src/book_ticker_update.cpp -o orderbook_tests

# Run
./orderbook_demo
./orderbook_tests
```

## API Documentation

### OrderBook Class

```cpp
class OrderBook {
public:
    // Constructor
    explicit OrderBook(std::string_view symbol);
    
    // Update methods
    void UpdateBookTicker(const BookTickerUpdate& data);
    void UpdateDepth(const DepthUpdate& data);
    
    // Query methods
    std::optional<std::pair<std::pair<double, double>, std::pair<double, double>>> GetBestBidAsk() const;
    std::string ToString() const;
};
```

### JSON Message Formats

**BookTicker Update:**
```json
{
  "u":400900217,     // order book updateId
  "s":"BNBUSDT",     // symbol
  "b":"25.35190000", // best bid price
  "B":"31.21000000", // best bid qty
  "a":"25.36520000", // best ask price
  "A":"40.66000000"  // best ask qty
}
```

**Depth Update:**
```json
{
  "lastUpdateId": 160,
  "bids": [["25.00", "5"], ["24.50", "2"]],
  "asks": [["25.50", "3"], ["26.00", "7"]]
}
```

## Testing

Run the comprehensive test suite:

```bash
cd build
./orderbook_tests
```

Tests cover:
- JSON deserialization
- Order book operations
- Error handling
- Performance benchmarks

## Performance

- **Order Book Updates**: O(log n) complexity
- **Best Bid/Ask Retrieval**: O(1) complexity
- **Memory Efficient**: Uses STL containers optimally
- **Invalid Level Pruning**: Automatic removal of crossed orders

## Example Output

```
=== Order Book ===
[ 0] [ 5.00000 ] 25.00 | 25.40 [ 6.00000 ]
[ 1] [ 4.00000 ] 24.60 | 25.50 [ 3.00000 ]
[ 2] [ 2.00000 ] 24.50 | 26.00 [ 7.00000 ]
...

Best Bid: 25 Qty: 5
Best Ask: 25.4 Qty: 6
```

## Configuration

The order book can be configured for different symbols and depth levels by modifying the source code parameters.
