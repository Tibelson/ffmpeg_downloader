#include "utils.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
// #include <sstream>
#include <sys/stat.h>

namespace Utils {

std::string sanitize_filename(const std::string& name) {
    std::string result;
    result.reserve(name.size());
    for (char c : name) {
        // Keep alphanumeric, spaces, dots, dashes, underscores, parentheses
        if (std::isalnum(static_cast<unsigned char>(c)) ||
            c == ' ' || c == '.' || c == '-' || c == '_' ||
            c == '(' || c == ')' || c == '[' || c == ']') {
            result += c;
        } else {
            result += '_';
        }
    }
    // Collapse consecutive underscores/spaces
    std::string cleaned;
    char prev = 0;
    for (char c : result) {
        if ((c == '_' || c == ' ') && (prev == '_' || prev == ' ')) continue;
        cleaned += c;
        prev = c;
    }
    // Trim trailing dots and spaces
    while (!cleaned.empty() && (cleaned.back() == '.' || cleaned.back() == ' ')) {
        cleaned.pop_back();
    }
    return cleaned.empty() ? "download" : cleaned;
}

std::string shell_escape(const std::string& arg) {
    // Wrap in single quotes; escape embedded single-quotes as '\''
    std::string result = "'";
    for (char c : arg) {
        if (c == '\'') {
            result += "'\\''";
        } else {
            result += c;
        }
    }
    result += "'";
    return result;
}

std::string get_extension(const std::string& url) {
    // Strip query string first
    std::string path = url;
    auto qpos = path.find('?');
    if (qpos != std::string::npos) path = path.substr(0, qpos);
    auto fragpos = path.find('#');
    if (fragpos != std::string::npos) path = path.substr(0, fragpos);

    auto dot = path.rfind('.');
    auto slash = path.rfind('/');
    if (dot == std::string::npos) return "";
    if (slash != std::string::npos && dot < slash) return "";

    std::string ext = path.substr(dot + 1);
    // Lower-case it
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return ext;
}

std::string url_basename(const std::string& url) {
    // Strip query string
    std::string path = url;
    auto qpos = path.find('?');
    if (qpos != std::string::npos) path = path.substr(0, qpos);
    auto fragpos = path.find('#');
    if (fragpos != std::string::npos) path = path.substr(0, fragpos);

    auto slash = path.rfind('/');
    if (slash != std::string::npos) path = path.substr(slash + 1);

    return path.empty() ? "download" : path;
}

std::string expand_home(const std::string& path) {
    if (path.empty() || path[0] != '~') return path;
    const char* home = std::getenv("HOME");
    if (!home) return path;
    return std::string(home) + path.substr(1);
}

bool directory_exists(const std::string& path) {
    struct stat st{};
    return (stat(path.c_str(), &st) == 0) && S_ISDIR(st.st_mode);
}

bool make_directories(const std::string& path) {
    // Simple recursive mkdir -p equivalent
    std::string cmd = "mkdir -p " + shell_escape(path);
    return std::system(cmd.c_str()) == 0;
}

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\n\r\f\v");
    return s.substr(start, end - start + 1);
}

bool icontains(const std::string& haystack, const std::string& needle) {
    auto it = std::search(
        haystack.begin(), haystack.end(),
        needle.begin(), needle.end(),
        [](unsigned char a, unsigned char b){
            return std::tolower(a) == std::tolower(b);
        }
    );
    return it != haystack.end();
}

} // namespace Utils
