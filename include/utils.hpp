#pragma once

#include <string>
// #include <vector>

namespace Utils {

/// Replace or strip characters that are illegal in filenames.
std::string sanitize_filename(const std::string& name);

/// Wrap a string in single-quotes for safe shell execution.
/// Properly escapes embedded single-quotes.
std::string shell_escape(const std::string& arg);

/// Extract the file extension from a URL path (e.g. "mp4", "m3u8").
/// Returns empty string if none found.
std::string get_extension(const std::string& url);

/// Extract the base filename portion of a URL (without query string).
std::string url_basename(const std::string& url);

/// Expand a leading '~' to the user's home directory.
std::string expand_home(const std::string& path);

/// Return true if the path exists as a directory.
bool directory_exists(const std::string& path);

/// Create directory (and parents). Returns false on failure.
bool make_directories(const std::string& path);

/// Strip leading/trailing whitespace from a string.
std::string trim(const std::string& s);

/// Case-insensitive string contains check.
bool icontains(const std::string& haystack, const std::string& needle);

} // namespace Utils
