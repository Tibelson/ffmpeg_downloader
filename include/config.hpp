#pragma once

// #include <string>

namespace Config {

constexpr const char* VERSION       = "0.1.0";
constexpr const char* APP_NAME      = "ffmpeg_downloader";
constexpr const char* USER_AGENT    =
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) "
    "AppleWebKit/537.36 (KHTML, like Gecko) "
    "Chrome/120.0.0.0 Safari/537.36";

// Default output directory (resolved at runtime to handle ~)
constexpr const char* DEFAULT_OUTPUT_DIR = "~/Downloads";

// Default format when remuxing
constexpr const char* DEFAULT_FORMAT = "mp4";

// HTTP timeout in seconds
constexpr long HTTP_TIMEOUT_SECONDS = 30;
constexpr long HTTP_MAX_REDIRECTS   = 10;

} // namespace Config
