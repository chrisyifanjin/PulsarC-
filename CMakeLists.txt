cmake_minimum_required(VERSION 3.15)
project(BinanceOrderBook VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler options
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
endif()

# Include directories
include_directories(include)
include_directories(third_party)

# ============================================================================
# Dependencies
# ============================================================================

# Google Test - using FetchContent (modern CMake approach)
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    GIT_SHALLOW TRUE
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# nlohmann/json is header-only, included in third_party/
# No additional setup needed due to include_directories above

# ============================================================================
# Core Library
# ============================================================================

# Source files for the core library
set(LIB_SOURCES
    src/order_book.cpp
    src/depth_update.cpp
    src/book_ticker_update.cpp
)

# Create static library
add_library(orderbook_lib STATIC ${LIB_SOURCES})

# Link dependencies to library
target_include_directories(orderbook_lib PUBLIC include)
target_include_directories(orderbook_lib PRIVATE third_party)

# ============================================================================
# Executables and Tests
# ============================================================================

enable_testing()

# Demo executable
add_executable(orderbook_demo src/main.cpp)
target_link_libraries(orderbook_demo orderbook_lib)

# Test executable
add_executable(orderbook_tests tests/test_orderbook.cpp)
target_link_libraries(orderbook_tests orderbook_lib gtest gtest_main)

# Register tests with CTest
add_test(NAME OrderBookTests COMMAND orderbook_tests)

# ============================================================================
# Installation
# ============================================================================

# Install targets
install(TARGETS orderbook_demo orderbook_tests orderbook_lib
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
