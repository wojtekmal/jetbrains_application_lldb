#pragma once
#include <cstdint>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

struct SymbolInfo
{
    uintptr_t address;
    uint64_t size;
    std::optional<bool> is_signed;
};

std::optional<uintptr_t> get_aslr_offset(const fs::path& path, pid_t pid);
std::optional<SymbolInfo> get_symbol_info(const std::string& symbol, const fs::path& executable_path, pid_t pid);
std::optional<SymbolInfo> read_elf(const std::string& symbol, const fs::path& executable_path);
