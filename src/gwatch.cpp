#include <vector>
#include <string>
#include "watcher.h"
#include <iostream>

int main(int argc, char** argv)
{
    std::vector<std::string> args(argv, argv + argc);

    try
    {
        watch(args);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Welp, there was an error. Here it is:\n" << e.what() << "\n";
        return EXIT_FAILURE;
    }
}