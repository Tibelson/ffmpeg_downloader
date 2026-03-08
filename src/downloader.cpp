#include "downloader.hpp"
#include "extractor/generic_extractor.hpp"
#include "ffmpeg_runner.hpp"
#include "utils.hpp"
#include "logger.hpp"
#include "config.hpp"

#include <iostream>
// #include <iomanip>
// #include <stdexcept>

namespace Downloader {

static std::string choose_output_format(const StreamInfo& stream,
                                         const std::string& requested,
                                         bool audio_only) {
    if (audio_only) return "mp3";
    if (!requested.empty()) return requested;
    // For HLS/DASH default to mp4; for direct files keep original extension
    if (stream.type == StreamType::HLS || stream.type == StreamType::DASH) {
        return Config::DEFAULT_FORMAT;
    }
    return stream.format.empty() ? Config::DEFAULT_FORMAT : stream.format;
}

static void print_stream(const StreamInfo& s, int idx) {
    std::cout << "  [" << idx << "] "
              << s.type_str() << " | "
              << s.format    << " | "
              << (s.quality > 0 ? std::to_string(s.quality) + "p" : "unknown quality")
              << "\n      " << s.url << "\n";
}

int run(const Options& opts) {
    Logger::verbose = opts.verbose;

    // --- Ensure output directory exists (skip for info-only) ---
    if (!opts.info_only && !opts.output_dir.empty()) {
        if (!Utils::directory_exists(opts.output_dir)) {
            Logger::step("Creating output directory: " + opts.output_dir);
            if (!Utils::make_directories(opts.output_dir)) {
                Logger::error("Failed to create directory: " + opts.output_dir);
                return 1;
            }
        }
    }

    // --- Extract streams ---
    Logger::info("Analyzing: " + opts.url);
    GenericExtractor extractor;
    std::vector<StreamInfo> streams;
    try {
        streams = extractor.extract(opts.url);
    } catch (const std::exception& ex) {
        Logger::error(std::string("Extraction failed: ") + ex.what());
        return 1;
    }

    if (streams.empty()) {
        Logger::error("No downloadable media streams found for: " + opts.url);
        Logger::warn("The page may require JavaScript or authentication.");
        return 1;
    }

    // --- Print info ---
    std::cout << "\nDetected streams (" << streams.size() << "):\n";
    for (size_t i = 0; i < streams.size(); ++i) {
        print_stream(streams[i], static_cast<int>(i));
    }
    std::cout << "\n";

    if (opts.info_only) {
        return 0; // Stop here for 'info' command
    }

    // --- Pick best stream (first = highest priority: HLS > DASH > DIRECT) ---
    const StreamInfo& best = streams[0];
    Logger::info("Selected: " + best.type_str() + " stream");

    // --- Build output path ---
    std::string out_fmt  = choose_output_format(best, opts.format, opts.audio_only);
    std::string title    = best.title.empty()
                               ? Utils::sanitize_filename(Utils::url_basename(opts.url))
                               : Utils::sanitize_filename(best.title);
    std::string filename = title + "." + out_fmt;
    std::string out_path = opts.output_dir + "/" + filename;

    Logger::info("Output: " + out_path);

    // --- Run ffmpeg ---
    FfmpegRunner::RunOptions ro;
    ro.stream      = best;
    ro.output_path = out_path;
    ro.audio_only  = opts.audio_only;
    ro.verbose     = opts.verbose;

    int exit_code = 0;
    try {
        exit_code = FfmpegRunner::run(ro);
    } catch (const std::exception& ex) {
        Logger::error(std::string("ffmpeg error: ") + ex.what());
        return 1;
    }

    if (exit_code == 0) {
        Logger::success("Saved: " + out_path);
    } else {
        Logger::error("ffmpeg exited with code " + std::to_string(exit_code));
    }

    return exit_code;
}

} // namespace Downloader
