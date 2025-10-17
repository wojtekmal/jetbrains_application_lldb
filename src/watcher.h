#include "types.h"
#include <vector>

uint64_t get_current_value(SymbolInfo symbol_info, pid_t pid);
bool stopped_because_of_read(pid_t pid);
int watch(const std::vector<std::string>& args);