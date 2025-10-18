#include <gtest/gtest.h>
#include "elf_parser.h"

const std::filesystem::path test_bin_dir = TEST_BIN_DIR;

TEST(ElfParserTest, HandlesNonexistantExecutable)
{
    auto result = get_symbol_info("name_of_symbol_is_irrelevant", "/nonexistant/path", 12345);
    EXPECT_FALSE(result.has_value());
}

TEST(ElfParserTest, CorrectlyFindsSymbolAddress)
{
    auto executable = test_bin_dir / "test_int_incrementation";
    auto output = read_elf("global_int_to_watch", executable);
    const auto expected_output = SymbolInfo{0x4014, 4};
    // Based on output of the readelf bash command.

    ASSERT_TRUE(output.has_value());
    EXPECT_EQ(output->address, expected_output.address);
    EXPECT_EQ(output->size, expected_output.size);
}