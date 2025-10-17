#include "elf_parser.h"
#include <elf.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>

std::optional<uintptr_t> get_aslr_offset(const fs::path& executable_path, pid_t pid)
{
    std::ifstream maps_file("/proc/" + std::to_string(pid) + "/maps");
    if (!maps_file.is_open()) return std::nullopt;
    std::string line;

    while (getline(maps_file, line))
    {
        std::istringstream iss(line);
        std::string addr_range, perms, offset_str, dev, inode, backer_path;
        iss >> addr_range >> perms >> offset_str >> dev >> inode >> std::ws;
        getline(iss, backer_path);

        if (backer_path.empty()) continue;

        try
        {
            if (fs::equivalent(executable_path, backer_path))
            {
                std::string first_address_str = addr_range.substr(0, addr_range.find('-'));
                uintptr_t first_address = std::stoull(first_address_str, nullptr, 16);
                return first_address - std::stoull(offset_str, nullptr, 16);
            }
        }
        catch (const fs::filesystem_error&)
        {
            continue;
        }
    }

    return std::nullopt;
}

std::optional<SymbolInfo> get_symbol_info(const std::string& symbol, const fs::path& executable_path, pid_t pid)
{
    std::ifstream executable_file(executable_path, std::ios::binary);
    if (!executable_file.is_open()) return std::nullopt;
    std::vector<char> buffer(std::istreambuf_iterator<char>(executable_file), {});
    char* data = buffer.data();

    auto elf_header = *reinterpret_cast<Elf64_Ehdr*>(data);
    auto section_headers = reinterpret_cast<Elf64_Shdr*>(data + elf_header.e_shoff);
    Elf64_Shdr symbol_table_header;

    for (int i = 0; i < elf_header.e_shnum; i++)
    {
        if (section_headers[i].sh_type == SHT_SYMTAB)
        {
            symbol_table_header = section_headers[i];
            break;
        }
    }

    Elf64_Sym* symbol_table = reinterpret_cast<Elf64_Sym*>(data + symbol_table_header.sh_offset);
    char* symbol_string_table = data + section_headers[symbol_table_header.sh_link].sh_offset;
    int symbol_count = symbol_table_header.sh_size / sizeof(Elf64_Sym);

    for (int i = 0; i < symbol_count; i++)
    {
        if (symbol == symbol_string_table + symbol_table[i].st_name)
        {
            Elf64_Sym symbol_entry = symbol_table[i];

            auto aslr_offset_opt = get_aslr_offset(executable_path, pid);
            if (!aslr_offset_opt) return std::nullopt;

            uintptr_t real_address = *aslr_offset_opt + static_cast<uint64_t>(symbol_entry.st_value);
            return SymbolInfo{real_address, symbol_entry.st_size};
        }
    }

    return std::nullopt;
}