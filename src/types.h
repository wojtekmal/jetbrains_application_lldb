#pragma once
#include <cstdint>
#include <sys/types.h>
#include <optional>

struct SymbolInfo
{
    uintptr_t address;
    uint64_t size;
    std::optional<bool> is_signed;
};