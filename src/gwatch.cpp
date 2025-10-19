#include <vector>
#include <string>
#include "watcher.h"

int main(int argc, char** argv)
{
    std::vector<std::string> args(argv, argv + argc);
    watch(args);
}