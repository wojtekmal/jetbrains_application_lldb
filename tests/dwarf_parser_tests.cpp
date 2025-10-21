#include <gtest/gtest.h>
#include "dwarf_parser.h"
#include <filesystem>
#include <array>
#include <algorithm>
#include <sstream>

const std::filesystem::path test_bin_dir = TEST_BIN_DIR;

TEST(DwarfParserTest, ReadsULEB128Correctly)
{
    // Example from Wikipedia article on LEB128.
    constexpr int test_count = 2;

    std::array<uint64_t, test_count> uleb128_encodings =
    {{
        0b00000000'00100110'10001110'11100101,
        0b00000000'00000000'00000000'00000000
    }};

    std::array<uint64_t, test_count> encoded_numbers =
    {{
        624485,
        0
    }};

    for (int i = 0; i < test_count; i++)
    {
        uint64_t uleb128_encoding = uleb128_encodings[i];
        auto pointer = reinterpret_cast<uint8_t*>(&uleb128_encoding);
        uint64_t result = read_uleb128(pointer);
        EXPECT_EQ(result, encoded_numbers[i]);
        EXPECT_EQ(pointer, reinterpret_cast<uint8_t*>(&uleb128_encoding) + std::max(1, 8 - __builtin_clzll(uleb128_encoding) / 8));
    }
}

TEST(DwarfParserTest, ReadsSLEB128Correctly)
{
    // Example from Wikipedia article on LEB128.
    constexpr int test_count = 2;

    std::array<int64_t, test_count> sleb128_encodings =
    {{
        0b00000000'01111000'10111011'11000000,
        0b00000000'00000000'00000000'00000000
    }};

    std::array<int64_t, test_count> encoded_numbers =
    {{
        -123456,
        0
    }};

    for (int i = 0; i < test_count; i++)
    {
        int64_t sleb128_encoding = sleb128_encodings[i];
        auto pointer = reinterpret_cast<uint8_t*>(&sleb128_encoding);
        int64_t result = read_sleb128(pointer);
        EXPECT_EQ(result, encoded_numbers[i]);
        EXPECT_EQ(pointer, reinterpret_cast<uint8_t*>(&sleb128_encoding) + std::max(1, 8 - __builtin_clzll(sleb128_encoding) / 8));
    }
}