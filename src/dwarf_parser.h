#include <optional>
#include <elf.h>
#include <string>

std::optional<bool> get_sign_info(const std::string& symbol, char* data, const std::optional<Elf64_Shdr>& debug_info_header_opt, const std::optional<Elf64_Shdr>& debug_abbrev_header_opt);