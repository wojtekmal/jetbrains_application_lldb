# CMake generated Testfile for 
# Source directory: /home/wojtekmal/informatyka/internships/jetbrains_application_lldb
# Build directory: /home/wojtekmal/informatyka/internships/jetbrains_application_lldb/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ElfParserTest.HandlesNonexistantExecutable "/home/wojtekmal/informatyka/internships/jetbrains_application_lldb/build/bin/run_tests" "--gtest_filter=ElfParserTest.HandlesNonexistantExecutable")
set_tests_properties(ElfParserTest.HandlesNonexistantExecutable PROPERTIES  DEF_SOURCE_LINE "tests/elf_parser_tests.cpp:4" SKIP_REGULAR_EXPRESSION "\\[  SKIPPED \\]" _BACKTRACE_TRIPLES "/usr/share/cmake-3.31/Modules/GoogleTest.cmake:510:EVAL;2;add_test;/usr/share/cmake-3.31/Modules/GoogleTest.cmake:510:EVAL;0;;/usr/share/cmake-3.31/Modules/GoogleTest.cmake;510;cmake_language;/home/wojtekmal/informatyka/internships/jetbrains_application_lldb/CMakeLists.txt;56;gtest_add_tests;/home/wojtekmal/informatyka/internships/jetbrains_application_lldb/CMakeLists.txt;0;")
add_test(TracerIntegrationTest.TracesIntegerWritesCorrectly "/home/wojtekmal/informatyka/internships/jetbrains_application_lldb/build/bin/run_tests" "--gtest_filter=TracerIntegrationTest.TracesIntegerWritesCorrectly")
set_tests_properties(TracerIntegrationTest.TracesIntegerWritesCorrectly PROPERTIES  DEF_SOURCE_LINE "tests/watcher_tests.cpp:8" SKIP_REGULAR_EXPRESSION "\\[  SKIPPED \\]" _BACKTRACE_TRIPLES "/usr/share/cmake-3.31/Modules/GoogleTest.cmake:510:EVAL;2;add_test;/usr/share/cmake-3.31/Modules/GoogleTest.cmake:510:EVAL;0;;/usr/share/cmake-3.31/Modules/GoogleTest.cmake;510;cmake_language;/home/wojtekmal/informatyka/internships/jetbrains_application_lldb/CMakeLists.txt;56;gtest_add_tests;/home/wojtekmal/informatyka/internships/jetbrains_application_lldb/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
