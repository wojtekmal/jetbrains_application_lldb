#include <gtest/gtest.h>
#include "watcher.h"
#include <filesystem>
#include <string>
#include <iostream>

const std::filesystem::path test_bin_dir = TEST_BIN_DIR;

class OutputInterceptor
{
private:
    std::streambuf* original_buffer;

public:
    OutputInterceptor(std::stringstream& stream) : original_buffer(std::cout.rdbuf())
    {
        std::cout.rdbuf(stream.rdbuf());
    }

    ~OutputInterceptor()
    {
        std::cout.rdbuf(original_buffer);
    }
};

TEST(TracerIntegrationTest, TracesIntegerWritesCorrectly) {
    auto test_program_path = test_bin_dir / "test_int_incrementation";
    std::string symbol_to_watch = "global_int_to_watch";

    std::stringstream interceptor_buffer;
    OutputInterceptor interceptor(interceptor_buffer);
    watch({"gwatch", "--var", symbol_to_watch, "--exec", test_program_path});

    std::string expected_output = R"(global_int_to_watch    read     0
global_int_to_watch    write    0 -> 1
global_int_to_watch    read     1
global_int_to_watch    write    1 -> 2
global_int_to_watch    read     2
global_int_to_watch    write    2 -> 3
global_int_to_watch    read     3
global_int_to_watch    write    3 -> 4
global_int_to_watch    read     4
global_int_to_watch    write    4 -> 5
)";

    EXPECT_EQ(interceptor_buffer.str(), expected_output);
}