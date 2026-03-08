#include "ffmpeg_runner.hpp"
#include "utils.hpp"
#include "logger.hpp"

// #include <array>
// #include <cstdio>
#include <cstdlib>
#include <sstream>
#include <stdexcept>

namespace FfmpegRunner {

std::string find_ffmpeg() {
#ifdef FFMPEG_PATH
    // Injected at compile time by CMakeLists.txt
    return std::string(FFMPEG_PATH);
#else
    // Runtime fallback: search PATH
    FILE* pipe = popen("which ffmpeg 2>/dev/null", "r");
    if (!pipe) return {};
    std::array<char, 256> buf{};
    std::string path;
    while (fgets(buf.data(), buf.size(), pipe) != nullptr) {
        path += buf.data();
    }
    pclose(pipe);
    // Trim trailing newline
    while (!path.empty() && (path.back() == '\n' || path.back() == '\r')) {
        path.pop_back();
    }
    return path;
#endif
}

int run(const RunOptions& opts) {
    std::string ffmpeg = find_ffmpeg();
    if (ffmpeg.empty()) {
        throw std::runtime_error(
            "ffmpeg not found. Install with: brew install ffmpeg");
    }

    // --- Build the command ---
    std::ostringstream cmd;
    cmd << Utils::shell_escape(ffmpeg);
    cmd << " -y";                          // Overwrite output without asking

    // Input
    cmd << " -i " << Utils::shell_escape(opts.stream.url);

    if (opts.audio_only) {
        // Extract audio only and encode to mp3
        cmd << " -vn -acodec libmp3lame -q:a 2";
    } else {
        // For HLS / DASH / direct: stream copy (no re-encode)
        cmd << " -c copy";
    }

    // Avoid prompts from ffmpeg
    cmd << " -loglevel " << (opts.verbose ? "info" : "warning");

    // Output
    cmd << " " << Utils::shell_escape(opts.output_path);

    std::string full_cmd = cmd.str();
    Logger::debug("Executing: " + full_cmd);
    Logger::step("Running ffmpeg...");

    // Run ffmpeg and stream its output to the terminal
    int ret = std::system(full_cmd.c_str());

    // std::system returns the shell exit status; extract actual exit code
#ifdef _WIN32
    return ret;
#else
    return WEXITSTATUS(ret);
#endif
}

} // namespace FfmpegRunner
