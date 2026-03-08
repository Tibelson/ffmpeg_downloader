#pragma once

#include <string>

namespace HttpClient {

struct Response {
    long        status_code = 0;
    std::string body;
    std::string effective_url;  // URL after redirects
    std::string content_type;
    bool        ok = false;     // true if status 200-299
};

/// Perform an HTTP GET request for the given URL.
/// Follows redirects, sets a browser-like User-Agent.
/// Throws std::runtime_error on curl initialization failure.
Response fetch(const std::string& url);

} // namespace HttpClient
