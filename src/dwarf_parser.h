#pragma once
#include <optional>
#include <elf.h>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <compare>

struct Abbrev
{
    struct Attribute
    {
        uint64_t name;
        uint64_t form;
    };

    uint64_t tag;
    uint64_t has_children;
    std::vector<Attribute> attributes;
};

struct CompilationUnitHeader
{
    uint64_t length;
    uint64_t version;
    uint64_t abbrev_offset;
    uint64_t offset_size;
    uint64_t address_size;
    std::unordered_map<uint64_t, Abbrev> abbrevs;
};

using Value = std::variant<
    uint64_t,
    int64_t,
    std::string
>;

struct AttributeData
{
    uint64_t form;
    Value value;
};

struct GetAttributesContext
{
    const std::vector<Abbrev::Attribute>& abbrev_attributes;
    uint64_t address_size;
    uint64_t offset_size;
};

struct GetNameContext
{
    std::unordered_map<uint64_t, AttributeData>& attributes;
    const std::optional<uint8_t*>& debug_str_opt;
};

struct DieDfsContext
{
    const std::string& symbol;
    const std::unordered_map<uint64_t, Abbrev>& abbrev_map;
    const CompilationUnitHeader& header;
    const std::optional<uint8_t*>& debug_str_opt;
    uint8_t*& start_of_compilation_unit;
};

uint64_t read_uleb128(uint8_t*& data);

int64_t read_sleb128(uint8_t*& data);

uint64_t read_bytes(uint8_t*& data, uint64_t byte_count);

std::string read_string(uint8_t*& data);

std::pair<uint64_t, Abbrev> read_abbrev(uint8_t*& data);

CompilationUnitHeader read_compilation_unit_header(uint8_t*& data);

std::unordered_map<uint64_t, AttributeData> get_attributes(uint8_t*& data, const GetAttributesContext& context);

std::string get_name(const GetNameContext& context);

std::optional<bool> die_dfs(uint8_t*& data, const DieDfsContext& context);

std::optional<bool> get_sign_info(const std::string& symbol, uint8_t* data, const std::optional<Elf64_Shdr>& debug_info_header_opt, const std::optional<Elf64_Shdr>& debug_abbrev_header_opt, const std::optional<Elf64_Shdr>& debug_str_header_opt);
