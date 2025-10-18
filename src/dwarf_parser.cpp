#include <libdwarf/dwarf.h>
#include <dwarf_parser.h>
#include <vector>
#include <unordered_map>
#include <utility>

struct Abbrev
{
public:
    struct Attribute
    {
        uint64_t name;
        uint64_t form;
    };

    uint64_t tag;
    uint64_t has_children;
    std::vector<Attribute> attributes;
};

uint64_t read_uleb128(char*& data)
{
    uint64_t answer = 0;
    uint64_t shift = 0;

    while (true)
    {
        unsigned char byte = data[shift];
        answer += (static_cast<uint64_t>(byte & 0x7f)) << (shift * 7);

        if (!(byte & 0x80))
        {
            shift++;
            break;
        }

        shift++;
    }

    data += shift;
    return answer;
}

uint64_t read_byte(char*& data)
{
    unsigned char answer = *data;
    data++;
    return static_cast<uint64_t>(answer);
}

std::pair<uint64_t, Abbrev> read_abbrev(char*& data)
{
    Abbrev abbrev;
    uint64_t code = read_uleb128(data);
    abbrev.tag = read_uleb128(data);
    abbrev.has_children = read_byte(data);

    Abbrev::Attribute attribute;

    while(true)
    {
        attribute.name = read_uleb128(data);
        attribute.form = read_uleb128(data);

        if (attribute.name == 0 && attribute.form == 0) break;
        else abbrev.attributes.push_back(attribute);
    }

    return {code, abbrev};
}

struct CompilationUnitHeader
{
    uint64_t length;
    uint64_t version;
    uint64_t abbrev_list_offset;
    uint64_t address_size;
    std::unordered_map<uint64_t, Abbrev> abbrevs;
};

CompilationUnitHeader read_compilation_unit_header(char*& data)
{
    if (*reinterpret_cast<uint32_t*>(data) == 0xffffffffU)
    {
        
    }
}

std::optional<bool> get_sign_info(const std::string& symbol, char* data, const std::optional<Elf64_Shdr>& debug_info_header_opt, const std::optional<Elf64_Shdr>& debug_abbrev_header_opt)
{
    if (!debug_info_header_opt.has_value() || !debug_abbrev_header_opt.has_value())
    {
        return std::nullopt;
    }

    char* debug_info = data + debug_info_header_opt->sh_offset;
    char* debug_abbrev = data + debug_abbrev_header_opt->sh_offset;

    
}