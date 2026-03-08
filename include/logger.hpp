#pragma once

#include <iostream>
#include <string>

// ANSI escape codes
#define ANSI_RESET  "\033[0m"
#define ANSI_BOLD   "\033[1m"
#define ANSI_RED    "\033[31m"
#define ANSI_GREEN  "\033[32m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_CYAN   "\033[36m"
#define ANSI_GRAY   "\033[90m"

namespace Logger {

// Global verbose flag — set at startup from CLI flag
extern bool verbose;

inline void info(const std::string& msg) {
    std::cout << ANSI_CYAN << "[INFO] " << ANSI_RESET << msg << "\n";
}

inline void success(const std::string& msg) {
    std::cout << ANSI_GREEN << ANSI_BOLD << "[✓] " << ANSI_RESET << msg << "\n";
}

inline void warn(const std::string& msg) {
    std::cerr << ANSI_YELLOW << "[WARN] " << ANSI_RESET << msg << "\n";
}

inline void error(const std::string& msg) {
    std::cerr << ANSI_RED << ANSI_BOLD << "[ERROR] " << ANSI_RESET << msg << "\n";
}

inline void debug(const std::string& msg) {
    if (verbose) {
        std::cout << ANSI_GRAY << "[DEBUG] " << ANSI_RESET << msg << "\n";
    }
}

inline void step(const std::string& msg) {
    std::cout << ANSI_CYAN << "  → " << ANSI_RESET << msg << "\n";
}

} // namespace Logger
