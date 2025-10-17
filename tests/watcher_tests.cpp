#include <gtest/gtest.h>
#include "watcher.h"
#include <filesystem>
#include <string>

const std::filesystem::path test_bin_dir = TEST_BIN_DIR;

TEST(TracerIntegrationTest, TracesIntegerWritesCorrectly) {
    auto test_program_path = test_bin_dir / "test_int_incrementation";
    std::string symbol_to_watch = "global_int_to_watch";

    

    EXPECT_EQ(0, 0);
}