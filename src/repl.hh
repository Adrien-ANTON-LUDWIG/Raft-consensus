#pragma once

#include <string>

#include "worlds_info.hh"

namespace REPL
{
    /// @brief Initialize REPL rank
    /// @param rank
    /// @param commandsFile Path to the file containing test commands. REPL will use headless mode if REPL commands are found.
    void init(Universe universe, const std::string &commandsFiles);

    /// @brief Launch REPL with CLI control
    void start(int clientCount, int serverCount);

    /// @brief Stop CLI control
    void stop();
} // namespace REPL