#pragma once

#include "stream_info.hpp"
#include <string>

namespace FfmpegRunner {

struct RunOptions {
    StreamInfo  stream;
    std::string output_path;   // Full path including filename with extension
    bool        audio_only = false;
    bool        verbose    = false;
};

/// Build and run an ffmpeg command for the given stream.
/// Returns the ffmpeg process exit code (0 = success).
/// Streams ffmpeg's stderr to the terminal in real time.
int run(const RunOptions& opts);

/// Locate the ffmpeg binary. Checks FFMPEG_PATH compile-time define first,
/// then falls back to searching PATH via 'which ffmpeg'.
/// Returns the path, or empty string if not found.
std::string find_ffmpeg();

} // namespace FfmpegRunner
