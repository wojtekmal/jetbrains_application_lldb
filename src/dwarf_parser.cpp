#include "dwarf.h"
#include "dwarf_parser.h"
#include <unordered_map>
#include <utility>
#include <cstring>
#include <variant>
#include <stdexcept>
#include <set>
#include <iostream>

uint64_t read_uleb128(uint8_t*& data)
{
    uint64_t answer = 0;
    int shift = 0;

    while (true)
    {
        uint8_t byte = *data;
        data++;
        answer += (static_cast<uint64_t>(byte & 0x7f)) << shift;
        shift += 7;

        if (!(byte & 0x80)) break;
    }

    return answer;
}

int64_t read_sleb128(uint8_t*& data)
{
    uint64_t contents;
    int shift = 0;

    while (true)
    {
        uint8_t byte = *data;
        data++;
        contents |= (static_cast<uint64_t>(byte & 0x7f)) << shift;
        shift += 7;

        if (!(byte & 0x80)) break;
    }

    if (contents & (1ULL << (shift - 1)))
    {
        contents |= ~((1ULL << shift) - 1);
    }

    return static_cast<int64_t>(contents);
}

uint64_t read_bytes(uint8_t*& data, uint64_t byte_count)
{
    uint64_t answer = 0;
    memcpy(&answer, data, byte_count);
    data += byte_count;
    return answer;
}

std::string read_string(uint8_t*& data)
{
    std::string answer;

    while (*data)
    {
        answer.push_back(static_cast<char>(*data));
        data++;
    }

    data++;
    return answer;
}

std::pair<uint64_t, Abbrev> read_abbrev(uint8_t*& data)
{
    Abbrev abbrev;
    uint64_t code = read_uleb128(data);
    abbrev.tag = read_uleb128(data);
    abbrev.has_children = read_bytes(data, 1);

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

CompilationUnitHeader read_compilation_unit_header(uint8_t*& data)
{
    CompilationUnitHeader answer;

    uint32_t length_32 = read_bytes(data, 4);

    if (length_32 == 0xffffffffU)
    {
        answer.length = read_bytes(data, 8);
    }
    else
    {
        answer.length = length_32;
    }

    answer.version = read_bytes(data, 2);
    answer.abbrev_offset = read_bytes(data, 8);
    answer.address_size = read_bytes(data, 1);
    return answer;
}

std::unordered_map<uint64_t, AttributeData> get_attributes(uint8_t*& data, const GetAttributesContext& context)
{
    std::unordered_map<uint64_t, AttributeData> attributes;

    for (const Abbrev::Attribute& attr : context.abbrev_attributes)
    {
        // I didn't have the time to write a 42 case switch statement, so I vibe coded it.
        Value attr_value;
        
        switch (attr.form)
        {
            // --- 1. Fixed-size Unsigned Data ---
            case DW_FORM_flag:
            case DW_FORM_data1:
            case DW_FORM_ref1:
                attr_value = read_bytes(data, 1);
                break;

            case DW_FORM_data2:
            case DW_FORM_ref2:
                attr_value = read_bytes(data, 2);
                break;

            case DW_FORM_data4:
            case DW_FORM_ref4:
            case DW_FORM_ref_sup4: // DWARF5: 4-byte offset
                attr_value = read_bytes(data, 4);
                break;

            case DW_FORM_data8:
            case DW_FORM_ref8:
            case DW_FORM_ref_sig8: // 8-byte signature
            case DW_FORM_ref_sup8: // DWARF5: 8-byte offset
                attr_value = read_bytes(data, 8);
                break;

            // --- 2. Variable-size Unsigned Data (ULEB128) ---
            case DW_FORM_udata:
            case DW_FORM_ref_udata:
            // DWARF5: These are ULEB128 indices into new tables
            case DW_FORM_strx:
            case DW_FORM_addrx:
            case DW_FORM_loclistx:
            case DW_FORM_rnglistx:
            case DW_FORM_strx1:
            case DW_FORM_strx2:
            case DW_FORM_strx3:
            case DW_FORM_strx4:
            case DW_FORM_addrx1:
            case DW_FORM_addrx2:
            case DW_FORM_addrx3:
            case DW_FORM_addrx4:
                attr_value = read_uleb128(data);
                break;

            // --- 3. Variable-size Signed Data (SLEB128) ---
            case DW_FORM_sdata:
                attr_value = read_sleb128(data);
                break;

            // --- 4. In-line String ---
            case DW_FORM_string:
                attr_value = read_string(data);
                break;

            // --- 5. Address / Offset (store as uint64_t) ---
            case DW_FORM_addr:
            case DW_FORM_ref_addr: // In DWARF 3+, size is address_size
                attr_value = read_bytes(data, context.address_size);
                break;
                
            case DW_FORM_strp:
            case DW_FORM_sec_offset:
            case DW_FORM_line_strp:
            case DW_FORM_strp_sup:
                // These are offsets, size is defined by the DWARF format
                attr_value = read_bytes(data, context.offset_size);
                break;

            // --- 6. Blocks (Cannot be stored in your Value) ---
            // Your variant<u64, s64, string> cannot hold a block.
            // You must skip them.
            case DW_FORM_block:
                {
                    uint64_t len = read_uleb128(data);
                    data += len;
                    continue; // Skip this attribute
                }
            case DW_FORM_block1:
                {
                    uint64_t len = read_bytes(data, 1);
                    data += len;
                    continue; // Skip this attribute
                }
            case DW_FORM_block2:
                {
                    uint64_t len = read_bytes(data, 2);
                    data += len;
                    continue; // Skip this attribute
                }
            case DW_FORM_block4:
                {
                    uint64_t len = read_bytes(data, 4);
                    data += len;
                    continue; // Skip this attribute
                }
            case DW_FORM_exprloc:
                {
                    uint64_t len = read_uleb128(data);
                    data += len;
                    continue; // Skip this attribute
                }
            case DW_FORM_data16:
                data += 16;
                continue; // Skip this attribute

            // --- 7. Special Forms ---
            case DW_FORM_flag_present:
                // No data in stream, just implies "true"
                attr_value = uint64_t(1);
                break;

            case DW_FORM_implicit_const:
                // The value is in the abbreviation, not the data stream.
                // You must handle this in the *abbreviation* parser.
                // We skip it here, as there's no data to read.
                continue; // Skip this attribute
                
            case DW_FORM_indirect:
                // This is a special case. You must read a ULEB128 *form code*
                // from the stream and then re-call this switch with that new form.
                // This is complex and left as an exercise.
                throw std::runtime_error("DW_FORM_indirect is not supported");

            default:
                // Unknown or unhandled form
                throw std::runtime_error("Unknown or unhandled DWARF form");
        }

        attributes[attr.name] = {attr.form, attr_value};
    }

    return attributes;
}

std::string get_name(const GetNameContext& context)
{
    auto attributes = context.attributes;
    std::string name;
    auto name_attribute = attributes.find(DW_AT_name);

    if (name_attribute != attributes.end())
    {
        Value value = name_attribute->second.value;

        switch (name_attribute->second.form)
        {
            case DW_FORM_string:
                name = std::get<std::string>(value);
                break;

            case DW_FORM_strp:
                name = (*context.debug_str_opt)[std::get<uint64_t>(value)];
                break;

            default:
                throw std::runtime_error(".debug_str_offsets is not supported.");
        }
    }

    return name;
}

std::optional<bool> die_dfs(uint8_t*& data, const DieDfsContext& context)
{
    uint64_t offset_size = context.header.abbrev_offset;
    uint64_t address_size = context.header.address_size;
    uint64_t abbrev_code = read_uleb128(data);
    Abbrev abbrev = context.abbrev_map.at(abbrev_code);
    auto attributes = get_attributes(data, GetAttributesContext{abbrev.attributes, address_size, offset_size});

    std::string name = get_name(GetNameContext{attributes, context.debug_str_opt});

    if (name == context.symbol && abbrev.tag == DW_TAG_variable)
    {
        uint8_t* type_die = context.start_of_compilation_unit + std::get<uint64_t>(attributes[DW_AT_type].value);
        uint64_t type_abbrev_code = read_uleb128(type_die);
        Abbrev type_abbrev = context.abbrev_map.at(type_abbrev_code);

        auto type_attributes = get_attributes(type_die, GetAttributesContext{type_abbrev.attributes, address_size, offset_size});
        std::string type_name = get_name(GetNameContext{type_attributes, context.debug_str_opt});

        // These lists were made up by a chatbot.
        std::set<std::string> signed_type_names = {
            "int",
            "signed int",
            "long",
            "long int",
            "signed long",
            "signed long int",
            "long long",
            "long long int",
            "signed long long",
            "signed long long int",
            "__int64" // GCC extension
        };

        std::set<std::string> unsigned_type_names = {
            "unsigned",
            "unsigned int",
            "long unsigned int",
            "unsigned long",
            "long long unsigned int",
            "unsigned long long",
            "unsigned __int64" // GCC extension
        };

        if (signed_type_names.count(type_name)) return true;
        if (unsigned_type_names.count(type_name)) return false;
        throw std::runtime_error("Type of variable is not supported.");
    }

    if (abbrev.has_children == DW_CHILDREN_no) return std::nullopt;
    
    while (*data)
    {
        auto result_from_child = die_dfs(data, context);
        if (result_from_child.has_value()) return result_from_child;
    }

    return std::nullopt;
}

std::optional<bool> get_sign_info(const std::string& symbol, uint8_t* data, const std::optional<Elf64_Shdr>& debug_info_header_opt, const std::optional<Elf64_Shdr>& debug_abbrev_header_opt, const std::optional<Elf64_Shdr>& debug_str_header_opt)
{
    if (!debug_info_header_opt.has_value() || !debug_abbrev_header_opt.has_value())
    {
        std::cerr << "Debug info is not present. Assuming that variable is unsigned. Compile with -g to get signed value of variable.\n";
        return std::nullopt;
    }

    uint8_t* debug_info = data + debug_info_header_opt->sh_offset;
    uint8_t* debug_abbrev = data + debug_abbrev_header_opt->sh_offset;
    std::optional<uint8_t*> debug_str_opt;

    if (debug_str_header_opt.has_value()) debug_str_opt = data + debug_str_header_opt->sh_offset;
    else debug_str_opt = std::nullopt;

    while(true)
    {
        uint8_t* start_of_compilation_unit = debug_info;
        CompilationUnitHeader header = read_compilation_unit_header(debug_info);
        std::unordered_map<uint64_t, Abbrev> abbrev_map;
        
        uint8_t* abbrev_list = debug_abbrev + header.abbrev_offset;

        while (*abbrev_list != 0)
        {
            abbrev_map.insert(read_abbrev(abbrev_list));
        }

        auto is_signed = die_dfs(debug_info, DieDfsContext{symbol, abbrev_map, header, debug_str_opt, start_of_compilation_unit});
    }
}