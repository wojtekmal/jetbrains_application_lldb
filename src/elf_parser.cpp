#include "elf_parser.h"
#include "dwarf_parser.h"
#include <elf.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>

std::optional<uint64_t> get_aslr_offset(const fs::path& executable_path, pid_t pid)
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
                uint64_t first_address = std::stoull(first_address_str, nullptr, 16);
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

std::optional<SymbolInfo> read_elf(const std::string& symbol, const fs::path& executable_path)
{
    std::ifstream executable_file(executable_path, std::ios::binary);
    if (!executable_file.is_open()) return std::nullopt;
    std::vector<char> buffer(std::istreambuf_iterator<char>(executable_file), {});
    auto data = reinterpret_cast<uint8_t*>(buffer.data());

    auto elf_header = *reinterpret_cast<Elf64_Ehdr*>(data);
    auto section_headers = reinterpret_cast<Elf64_Shdr*>(data + elf_header.e_shoff);
    auto section_name_string_table = reinterpret_cast<char*>(data + section_headers[elf_header.e_shstrndx].sh_offset);
    
    std::optional<Elf64_Shdr> symbol_table_header_opt;
    std::optional<Elf64_Shdr> debug_info_header_opt;
    std::optional<Elf64_Shdr> debug_abbrev_header_opt;
    std::optional<Elf64_Shdr> debug_str_header_opt;
    std::string debug_info_name = ".debug_info";
    std::string debug_abbrev_name = ".debug_abbrev";
    std::string debug_str_name = ".debug_str";

    for (int i = 0; i < elf_header.e_shnum; i++)
    {
        if (section_headers[i].sh_type == SHT_SYMTAB)
        {
            symbol_table_header_opt = section_headers[i];
        }
        if (section_name_string_table + section_headers[i].sh_name == debug_info_name)
        {
            debug_info_header_opt = section_headers[i];
        }
        if (section_name_string_table + section_headers[i].sh_name == debug_abbrev_name)
        {
            debug_abbrev_header_opt = section_headers[i];
        }
        if (section_name_string_table + section_headers[i].sh_name == debug_str_name)
        {
            debug_str_header_opt = section_headers[i];
        }
    }

    if (!symbol_table_header_opt.has_value()) return std::nullopt;
    auto symbol_table_header = *symbol_table_header_opt;
    auto symbol_is_signed = get_sign_info(symbol, data, debug_info_header_opt, debug_abbrev_header_opt, debug_str_header_opt);

    std::optional<SymbolInfo> symbol_info;

    Elf64_Sym* symbol_table = reinterpret_cast<Elf64_Sym*>(data + symbol_table_header.sh_offset);
    char* symbol_string_table = reinterpret_cast<char*>(data + section_headers[symbol_table_header.sh_link].sh_offset);
    int symbol_count = symbol_table_header.sh_size / sizeof(Elf64_Sym);

    for (int i = 0; i < symbol_count; i++)
    {
        if (symbol == symbol_string_table + symbol_table[i].st_name)
        {
            Elf64_Sym symbol_entry = symbol_table[i];
            symbol_info = SymbolInfo{static_cast<uintptr_t>(symbol_entry.st_value), symbol_entry.st_size, symbol_is_signed};
            break;
        }
    }

    return symbol_info;
}

std::optional<SymbolInfo> get_symbol_info(const std::string& symbol, const fs::path& executable_path, pid_t pid)
{
    auto aslr_offset_opt = get_aslr_offset(executable_path, pid);
    auto elf_data_opt = read_elf(symbol, executable_path);

    if (!aslr_offset_opt || !elf_data_opt) return std::nullopt;
    
    return SymbolInfo{elf_data_opt->address + *aslr_offset_opt, elf_data_opt->size, elf_data_opt->is_signed};
}