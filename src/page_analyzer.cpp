#include "page_analyzer.hpp"
#include "logger.hpp"
#include "utils.hpp"

#include <regex>
#include <set>
// #include <algorithm>

namespace PageAnalyzer {

// Known media file extensions
static const std::vector<std::string> DIRECT_EXTS = {
    "mp4", "webm", "mkv", "mov", "avi", "flv",
    "mp3", "aac", "ogg", "opus", "flac", "m4a", "wav"
};

std::string extract_title(const std::string& html) {
    // Case-insensitive match for <title>...</title>
    std::regex title_re(R"(<title[^>]*>([^<]+)</title>)",
                        std::regex::icase | std::regex::ECMAScript);
    std::smatch m;
    if (std::regex_search(html, m, title_re)) {
        return Utils::trim(m[1].str());
    }
    return {};
}

std::vector<StreamInfo> analyze(const std::string& effective_url,
                                const std::string& body) {
    std::vector<StreamInfo> results;
    std::set<std::string>   seen;   // deduplicate by URL

    std::string title = extract_title(body);
    if (title.empty()) {
        title = Utils::url_basename(effective_url);
    }
    Logger::debug("Page title: \"" + title + "\"");

    // --- 1. HLS manifests (.m3u8) ---
    {
        std::regex re(R"(https?://[^\s"'<>]+\.m3u8(?:[^\s"'<>]*))",
                      std::regex::icase | std::regex::ECMAScript);
        auto begin = std::sregex_iterator(body.begin(), body.end(), re);
        auto end   = std::sregex_iterator();
        for (auto it = begin; it != end; ++it) {
            std::string url = (*it)[0].str();
            if (seen.insert(url).second) {
                StreamInfo si;
                si.url    = url;
                si.title  = title;
                si.format = "m3u8";
                si.type   = StreamType::HLS;
                Logger::debug("Found HLS: " + url);
                results.push_back(std::move(si));
            }
        }
    }

    // --- 2. DASH manifests (.mpd) ---
    {
        std::regex re(R"(https?://[^\s"'<>]+\.mpd(?:[^\s"'<>]*))",
                      std::regex::icase | std::regex::ECMAScript);
        auto begin = std::sregex_iterator(body.begin(), body.end(), re);
        auto end   = std::sregex_iterator();
        for (auto it = begin; it != end; ++it) {
            std::string url = (*it)[0].str();
            if (seen.insert(url).second) {
                StreamInfo si;
                si.url    = url;
                si.title  = title;
                si.format = "mpd";
                si.type   = StreamType::DASH;
                Logger::debug("Found DASH: " + url);
                results.push_back(std::move(si));
            }
        }
    }

    // --- 3. Direct media file URLs ---
    {
        // Match full https?:// URLs ending in a known extension (optionally with query)
        std::regex re(R"(https?://[^\s"'<>]+\.(?:mp4|webm|mkv|mov|avi|flv|mp3|aac|ogg|opus|flac|m4a|wav)(?:\?[^\s"'<>]*)?)",
                      std::regex::icase | std::regex::ECMAScript);
        auto begin = std::sregex_iterator(body.begin(), body.end(), re);
        auto end   = std::sregex_iterator();
        for (auto it = begin; it != end; ++it) {
            std::string url = (*it)[0].str();
            if (seen.insert(url).second) {
                StreamInfo si;
                si.url    = url;
                si.title  = title;
                si.format = Utils::get_extension(url);
                si.type   = StreamType::DIRECT;
                Logger::debug("Found direct: " + url);
                results.push_back(std::move(si));
            }
        }
    }

    return results;
}

} // namespace PageAnalyzer
