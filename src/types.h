#pragma once
#include <cstdint>
#include <sys/types.h>

struct SymbolInfo
{
    uintptr_t address;
    uint64_t size;
};