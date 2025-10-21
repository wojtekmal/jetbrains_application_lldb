cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0135 NEW)

project(gwatch)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_library(
    gwatch_lib
    src/watcher.cpp
    src/elf_parser.cpp
    src/dwarf_parser.cpp
)

add_executable(
    gwatch
    src/gwatch.cpp
)

target_link_libraries(
    gwatch
    PRIVATE
    gwatch_lib
)

enable_testing()
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

add_executable(
    run_tests
    tests/elf_parser_tests.cpp
    tests/watcher_tests.cpp
    tests/dwarf_parser_tests.cpp
)

target_compile_definitions(run_tests PRIVATE
  "TEST_BIN_DIR=\"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\""
)

target_include_directories(run_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(
    run_tests
    PRIVATE
    gwatch_lib
    GTest::gtest_main
)

gtest_add_tests(TARGET run_tests)

add_executable(test_int_incrementation test_programs/test_int_incrementation.cpp)
add_executable(test_int_decrementation test_programs/test_int_decrementation.cpp)
target_compile_options(test_int_decrementation PRIVATE -g)
#target_compile_options(test_int_incrementation PRIVATE -O3)