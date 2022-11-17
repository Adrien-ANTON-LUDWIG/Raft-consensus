#pragma once

#include <string>

namespace REPL {
/// @brief Launch REPL with CLI control
void start();

/// @brief Stop CLI control
void stop();

/// @brief Execute command directly without CLI control
/// @param command
void headlessExec(const std::string& command);
}  // namespace REPL