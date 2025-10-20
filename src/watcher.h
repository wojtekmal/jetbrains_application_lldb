#pragma once
#include "elf_parser.h"
#include <vector>
#include <string>
#include <cstdint>

uint64_t get_current_value(SymbolInfo symbol_info, pid_t pid);
bool stopped_because_of_read(pid_t pid);
void watch(const std::vector<std::string>& args);