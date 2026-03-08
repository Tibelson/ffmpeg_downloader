#pragma once

#include <string>

/// Represents the type of a detected media stream.
enum class StreamType {
    DIRECT,   // Direct file URL (.mp4, .webm, .mp3, ...)
    HLS,      // Apple HTTP Live Streaming (.m3u8 manifest)
    DASH      // MPEG-DASH Adaptive Streaming (.mpd manifest)
};

/// Holds all information about a single detected media stream.
struct StreamInfo {
    std::string url;
    std::string title;          // Human-readable title (from page <title> or URL)
    std::string format;         // "mp4", "m3u8", "mpd", "webm", "mp3", ...
    StreamType  type = StreamType::DIRECT;
    int         quality = 0;    // Estimated quality: height (720) or bitrate (kbps). 0 = unknown.

    /// Return a human-readable type label.
    std::string type_str() const {
        switch (type) {
            case StreamType::HLS:  return "HLS";
            case StreamType::DASH: return "DASH";
            default:               return "DIRECT";
        }
    }
};
