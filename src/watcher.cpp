#include "watcher.h"
#include <cstdlib>
#include <sys/ptrace.h>
#include <cstddef>
#include <sys/user.h>
#include <iostream>
#include <unistd.h>
#include <wait.h>
#include <map>

uint64_t get_current_value(SymbolInfo symbol_info, pid_t pid)
{
    uint64_t answer = ptrace(PTRACE_PEEKDATA, pid, symbol_info.address, nullptr);

    if (symbol_info.size < 8)
    {
        answer &= (1ULL << (8 * symbol_info.size)) - 1;
    }

    return answer;
}

bool stopped_because_of_read(pid_t pid)
{
    uint64_t dr6 = ptrace(PTRACE_PEEKUSER, pid, offsetof(user, u_debugreg[6]), nullptr);
    if (dr6 & 2) return false;
    else return true;
}

std::string format(uint64_t value, const SymbolInfo& symbol_info)
{
    if (!symbol_info.is_signed.has_value() || *symbol_info.is_signed)
    {
        if (symbol_info.size == 4) return std::to_string(static_cast<uint32_t>(value));
        if (symbol_info.size == 8) return std::to_string(static_cast<uint64_t>(value));
    }
    else
    {
        if (symbol_info.size == 4) return std::to_string(static_cast<int32_t>(value));
        if (symbol_info.size == 8) return std::to_string(static_cast<int64_t>(value));
    }

    std::cerr << "Symbol size: " << symbol_info.size << "\n";
    throw std::runtime_error("Unsupported symbol size.");
}

void watch(const std::vector<std::string>& args)
{
    std::string symbol;
    std::string path;

    for (int i = 0; i < args.size(); i++)
    {
        if (args[i] == "--var" && i + 1 < args.size())
        {
            symbol = args[i + 1];
        }
        if (args[i] == "--exec" && i + 1 < args.size())
        {
            path = args[i + 1];
        }
    }

    if (symbol == "") throw std::runtime_error("--var was not given.");
    if (path == "") throw std::runtime_error("--exec was not given.");

    pid_t pid = fork();

    if (pid == 0)
    {
        ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
        char* args[2] = {path.data(), nullptr};
        execve(path.data(), args, nullptr);
    }
    else
    {
        waitpid(pid, nullptr, WUNTRACED);
        auto symbol_info_opt = get_symbol_info(symbol, path, pid);

        if (!symbol_info_opt)
        {
            std::cerr << "Couldn't find symbol " << symbol << " in executable " << path << "\n";
            throw std::runtime_error("Symbol not found.");
        }

        auto symbol_info = *symbol_info_opt;
        ptrace(PTRACE_POKEUSER, pid, offsetof(user, u_debugreg[0]), symbol_info.address);
        ptrace(PTRACE_POKEUSER, pid, offsetof(user, u_debugreg[1]), symbol_info.address);
        uint64_t current_value = get_current_value(symbol_info, pid);

        uint64_t dr7 = ptrace(PTRACE_PEEKUSER, pid, offsetof(user, u_debugreg[7]), nullptr);
        dr7 &= ~(0b11111111ULL << 16);
        std::map<uint64_t, uint64_t> size_masks = {{1, 0}, {2, 1}, {4, 3}, {8, 2}};
        dr7 |= (size_masks[symbol_info.size] << 18) + (3ULL << 16) + 3;
        dr7 |= (size_masks[symbol_info.size] << 22) + (1ULL << 20) + 12;

        while (true)
        {
            ptrace(PTRACE_POKEUSER, pid, offsetof(user, u_debugreg[7]), dr7);
            ptrace(PTRACE_CONT, pid, nullptr, nullptr);
            int status;
            waitpid(pid, &status, WUNTRACED);

            if (WIFSTOPPED(status))
            {
                if (stopped_because_of_read(pid))
                {
                    std::cout << symbol << "    read     " << format(current_value, symbol_info) << "\n";
                }
                else
                {
                    uint64_t new_value = get_current_value(symbol_info, pid);
                    std::cout << symbol << "    write    " << format(current_value, symbol_info) << " -> " << format(new_value, symbol_info) << "\n";
                    current_value = new_value;
                }
            }
            else
            {
                break;
            }
        }
    }
}