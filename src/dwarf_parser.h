#include <optional>
#include <elf.h>
#include <string>
#include <unordered_map>
#include <variant>

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

struct DieDfsContext
{
    const std::string& symbol;
    const std::unordered_map<uint64_t, Abbrev>& abbrev_map;
    const CompilationUnitHeader& header;
    const std::optional<uint8_t*>& debug_str_opt;
    uint8_t*& start_of_compilation_unit;
};

std::optional<bool> die_dfs(uint8_t*& data, const DieDfsContext& context);