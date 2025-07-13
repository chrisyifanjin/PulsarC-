# Binance OrderBook Implementation - Build Instructions

## Quick Start

### Prerequisites
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.15+

### Build Steps

```bash
# 1. Create build directory
mkdir build && cd build

# 2. Configure project
cmake ..

# 3. Build all targets
cmake --build . --parallel
```

### Run Demo
```bash
# From build directory
./orderbook_demo
```

### Run Tests
```bash
# From build directory  
./orderbook_tests

# Or using CTest for detailed output
ctest --verbose
```

## Project Features
- JSON deserialization for BookTicker and DepthUpdate messages
- High-performance OrderBook with O(log n) operations
- Automatic pruning of invalid price levels
- Comprehensive error handling
- Full Google Test suite (12 tests)
- Performance optimized for high-frequency updates

## Expected Output
The demo shows a formatted orderbook and the tests verify all functionality including edge cases and performance requirements.
