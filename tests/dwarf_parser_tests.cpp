#include <gtest/gtest.h>
#include "dwarf_parser.h"
#include <filesystem>
#include <array>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <utility>
#include "dwarf.h"

const std::filesystem::path test_bin_dir = TEST_BIN_DIR;

TEST(DwarfParserTest, ReadsULEB128Correctly)
{
    // Example from Wikipedia article on LEB128.
    constexpr int test_count = 2;

    std::array<uint64_t, test_count> uleb128_encodings =
    {{
        0b00100110'10001110'11100101,
        0
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
        0b01111000'10111011'11000000,
        0
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

TEST(DwarfParserTest, ReadsBytesCorrectly)
{
    std::array<uint64_t, 2> encodings =
    {{
        0x3a9f0b1e8d4c7a25,
        0xf1d8a0c2e7b39f4d
    }};

    for (uint64_t encoding : encodings)
    {
        for (int i = 1; i <= 8; i++)
        {
            auto pointer = reinterpret_cast<uint8_t*>(&encoding);
            uint64_t result = read_bytes(pointer, i);
            EXPECT_EQ(result, encoding & (~0ULL >> (64 - i * 8)));
            EXPECT_EQ(pointer, reinterpret_cast<uint8_t*>(&encoding) + i);
        }
    }
}

TEST(DwarfParserTest, ReadsStringCorrectly)
{
    std::array<const char*, 2> strings = 
    {{
        "abc",
        ""
    }};

    for (const char* string : strings)
    {
        auto pointer = reinterpret_cast<uint8_t*>(const_cast<char*>(string));
        std::string result = read_string(pointer);
        EXPECT_EQ(result, string);
        EXPECT_EQ(pointer, reinterpret_cast<uint8_t*>(const_cast<char*>(string)) + strlen(string) + 1);
    }
}

inline bool operator==(const Abbrev::Attribute& lhs, const Abbrev::Attribute& rhs)
{
    return lhs.name == rhs.name &&
           lhs.form == rhs.form;
}

inline bool operator==(const Abbrev& lhs, const Abbrev& rhs)
{
    return lhs.tag == rhs.tag &&
           lhs.has_children == rhs.has_children &&
           lhs.attributes == rhs.attributes;
}

TEST(DwarfParserTest, ReadsAbbrevCorrectly)
{
    auto executable_path = test_bin_dir / "test_int_decrementation";
    std::ifstream executable_file(executable_path, std::ios::binary);
    std::vector<char> buffer(std::istreambuf_iterator<char>(executable_file), {});
    auto data = reinterpret_cast<uint8_t*>(buffer.data());
    
    // According to the readelf bash command.
    uint64_t debug_abbrev_offset = 0x30e9;
    auto debug_abbrev = data + debug_abbrev_offset;
    auto start_of_debug_abbrev = debug_abbrev;
    
    auto first_abbrev = read_abbrev(debug_abbrev);
    auto second_abbrev = read_abbrev(debug_abbrev);
    auto third_abbrev = read_abbrev(debug_abbrev);

    std::pair<uint64_t, Abbrev> expected_abbrevs[] = {
        // Abbreviation Entry 1
        { 1, 
        {
            .tag = DW_TAG_compile_unit,
            .has_children = DW_CHILDREN_yes,
            .attributes = {
                {DW_AT_producer, DW_FORM_strp},
                {DW_AT_language, DW_FORM_data1},
                {DW_AT_name, DW_FORM_line_strp},
                {DW_AT_comp_dir, DW_FORM_line_strp},
                {DW_AT_low_pc, DW_FORM_addr},
                {DW_AT_high_pc, DW_FORM_data8},
                {DW_AT_stmt_list, DW_FORM_sec_offset}
            }
        }
        },

        // Abbreviation Entry 2
        { 2,
        {
            .tag = DW_TAG_variable,
            .has_children = DW_CHILDREN_no,
            .attributes = {
                {DW_AT_name, DW_FORM_strp},
                {DW_AT_decl_file, DW_FORM_data1},
                {DW_AT_decl_line, DW_FORM_data1},
                {DW_AT_decl_column, DW_FORM_data1},
                {DW_AT_type, DW_FORM_ref4},
                {DW_AT_external, DW_FORM_flag_present},
                {DW_AT_location, DW_FORM_exprloc}
            }
        }
        },

        // Abbreviation Entry 3
        { 3,
        {
            .tag = DW_TAG_base_type,
            .has_children = DW_CHILDREN_no,
            .attributes = {
                {DW_AT_byte_size, DW_FORM_data1},
                {DW_AT_encoding, DW_FORM_data1},
                {DW_AT_name, DW_FORM_string}
            }
        }
        }
    };

    EXPECT_EQ(first_abbrev, expected_abbrevs[0]);
    EXPECT_EQ(second_abbrev, expected_abbrevs[1]);
    EXPECT_EQ(third_abbrev, expected_abbrevs[2]);
}