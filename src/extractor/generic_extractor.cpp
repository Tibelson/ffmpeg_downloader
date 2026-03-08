#include "extractor/generic_extractor.hpp"
#include "http_client.hpp"
#include "page_analyzer.hpp"
#include "utils.hpp"
#include "logger.hpp"

#include <algorithm>
#include <stdexcept>

static const std::vector<std::string> MEDIA_EXTS = {
    "mp4", "webm", "mkv", "mov", "avi", "flv",
    "mp3", "aac", "ogg", "opus", "flac", "m4a", "wav",
    "m3u8", "mpd"
};

static bool has_media_extension(const std::string& url) {
    std::string ext = Utils::get_extension(url);
    return !ext.empty() &&
           std::find(MEDIA_EXTS.begin(), MEDIA_EXTS.end(), ext) != MEDIA_EXTS.end();
}

bool GenericExtractor::supports(const std::string& /*url*/) const {
    return true; // universal fallback
}

std::vector<StreamInfo> GenericExtractor::extract(const std::string& url) {
    // --- Case 1: the URL itself is a direct media file or manifest ---
    std::string ext = Utils::get_extension(url);
    if (has_media_extension(url)) {
        Logger::debug("URL has media extension '" + ext + "', using directly");
        StreamInfo si;
        si.url    = url;
        si.title  = Utils::sanitize_filename(Utils::url_basename(url));
        // Strip extension from title
        if (!si.title.empty() && si.title.find('.') != std::string::npos) {
            si.title = si.title.substr(0, si.title.rfind('.'));
        }
        si.format = ext;
        if (ext == "m3u8") {
            si.type = StreamType::HLS;
        } else if (ext == "mpd") {
            si.type = StreamType::DASH;
        } else {
            si.type = StreamType::DIRECT;
        }
        return {si};
    }

    // --- Case 2: fetch the page and scan it ---
    Logger::step("Fetching page: " + url);
    HttpClient::Response resp = HttpClient::fetch(url);
    if (!resp.ok) {
        throw std::runtime_error("HTTP " + std::to_string(resp.status_code) +
                                 " fetching: " + url);
    }

    Logger::step("Analyzing page content...");
    std::vector<StreamInfo> streams =
        PageAnalyzer::analyze(resp.effective_url, resp.body);

    if (streams.empty()) {
        Logger::warn("No media streams detected in page.");
    } else {
        Logger::info("Detected " + std::to_string(streams.size()) + " stream(s)");
    }

    return streams;
}
