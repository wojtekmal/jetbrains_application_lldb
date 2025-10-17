#include <gtest/gtest.h>
#include "elf_parser.h"

TEST(ElfParserTest, HandlesNonexistantExecutable)
{
    auto result = get_symbol_info("name_of_symbol_is_irrelevant", "/nonexistant/path", 12345);
    EXPECT_FALSE(result.has_value());
}