#include <gtest/gtest.h>
#include <cstdio>
#include <filesystem>

const std::string gwatch = GWATCH;
const std::filesystem::path test_bin_dir = TEST_BIN_DIR;

std::string execute_command(const char* command)
{
    char buffer[256];
    std::string result;

    FILE* pipe = popen(command, "r");

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        result += buffer;
    }

    pclose(pipe);
    return result;
}

TEST(InvocationTest, CLIToolExecutesCorrectly)
{
    std::string executable_path = test_bin_dir / "test_int_incrementation";
    std::string symbol = "global_int_to_watch";
    std::string command = gwatch + " --exec " + executable_path + " --var " + symbol;
    std::string invocation_result = execute_command(command.data());

    std::string expected_result = R"(global_int_to_watch    read     0
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

    EXPECT_EQ(expected_result, invocation_result);
}