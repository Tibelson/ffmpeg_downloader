#include <CLI/CLI.hpp>

#include "config.hpp"
#include "logger.hpp"
#include "downloader.hpp"
#include "utils.hpp"

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    CLI::App app{
        std::string(Config::APP_NAME) + " v" + Config::VERSION +
        "\nA C++ media downloader powered by libcurl + ffmpeg."
    };
    app.name(Config::APP_NAME);
    app.set_help_flag("-h,--help", "Show this help and exit");

    // ----- Global option -----
    bool verbose = false;
    app.add_flag("-v,--verbose", verbose, "Enable verbose/debug output");

    // =====================================================================
    // Subcommand: download
    // =====================================================================
    auto* cmd_dl = app.add_subcommand("download", "Download video or audio from a URL");
    cmd_dl->require_option(0); // options are optional

    std::string dl_url;
    cmd_dl->add_option("URL", dl_url, "URL of the video or page to download")
          ->required();

    std::string dl_output = Utils::expand_home(Config::DEFAULT_OUTPUT_DIR);
    cmd_dl->add_option("-o,--output", dl_output,
                       "Output directory (default: ~/Downloads)")
          ->type_name("DIR");

    std::string dl_format;
    cmd_dl->add_option("-f,--format", dl_format,
                       "Output container format (e.g. mp4, mkv, mp3)")
          ->type_name("FMT");

    bool dl_audio_only = false;
    cmd_dl->add_flag("-a,--audio-only", dl_audio_only,
                     "Extract audio only (saved as .mp3)");

    cmd_dl->callback([&]() {
        Logger::verbose = verbose;

        Downloader::Options opts;
        opts.url        = dl_url;
        opts.output_dir = Utils::expand_home(dl_output);
        opts.format     = dl_format;
        opts.audio_only = dl_audio_only;
        opts.verbose    = verbose;
        opts.info_only  = false;

        std::exit(Downloader::run(opts));
    });

    // =====================================================================
    // Subcommand: info
    // =====================================================================
    auto* cmd_info = app.add_subcommand("info", "Detect and list available streams (no download)");

    std::string info_url;
    cmd_info->add_option("URL", info_url, "URL to analyze")->required();

    cmd_info->callback([&]() {
        Logger::verbose = verbose;

        Downloader::Options opts;
        opts.url       = info_url;
        opts.info_only = true;
        opts.verbose   = verbose;

        std::exit(Downloader::run(opts));
    });

    // =====================================================================
    // Subcommand: version
    // =====================================================================
    auto* cmd_ver = app.add_subcommand("version", "Print version information");
    cmd_ver->callback([&]() {
        std::cout << Config::APP_NAME << " v" << Config::VERSION << "\n";
        std::cout << "Built with: libcurl, CLI11, nlohmann_json, ffmpeg\n";
        std::exit(0);
    });

    // Require at least one subcommand
    app.require_subcommand(1);

    CLI11_PARSE(app, argc, argv);
    return 0;
}