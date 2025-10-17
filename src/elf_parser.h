#pragma once
#include <cstdint>
#include <filesystem>
#include <optional>
#include "types.h"

namespace fs = std::filesystem;

std::optional<uintptr_t> get_aslr_offset(const fs::path& path, pid_t pid);
std::optional<SymbolInfo> get_symbol_info(const std::string& symbol, const fs::path& executable_path, pid_t pid);