#pragma once

#include <string>

namespace Downloader {

struct Options {
    std::string url;
    std::string output_dir;   // Resolved directory (~ already expanded)
    std::string format;       // Output container, e.g. "mp4" (empty = auto-detect)
    bool        audio_only = false;
    bool        verbose    = false;
    bool        info_only  = false;  // If true, print streams but don't download
};

/// Run the full download pipeline for the given options.
/// Returns 0 on success, non-zero on failure.
int run(const Options& opts);

} // namespace Downloader
