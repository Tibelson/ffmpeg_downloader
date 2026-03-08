#pragma once

#include "stream_info.hpp"
#include <string>
#include <vector>

namespace PageAnalyzer {

/// Analyze an HTTP response body (HTML or plain text) and the effective URL
/// to extract any media stream candidates.
///
/// Detection priority (highest first):
///   1. HLS manifests  (.m3u8 URLs)
///   2. DASH manifests (.mpd URLs)
///   3. Direct media files (.mp4 .webm .mkv .mov .avi .mp3 .aac .ogg .opus .flac)
///
/// The page <title> is extracted and assigned to found streams as their title.
std::vector<StreamInfo> analyze(const std::string& effective_url,
                                const std::string& body);

/// Extract the text content of the first <title>...</title> tag.
/// Returns an empty string if none found.
std::string extract_title(const std::string& html);

} // namespace PageAnalyzer
