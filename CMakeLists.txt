cmake_minimum_required(VERSION 3.20)

project(ffmpeg_downloader
  VERSION 0.1.0
  DESCRIPTION "CLI media downloader using libcurl + ffmpeg"
  LANGUAGES CXX
)

# ----------------------------
# C++ standard
# ----------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile_commands.json for IDE language servers (clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ----------------------------
# Executable
# ----------------------------
add_executable(ffmpeg_downloader
  src/main.cpp
  src/logger.cpp
  src/utils.cpp
  src/http_client.cpp
  src/page_analyzer.cpp
  src/ffmpeg_runner.cpp
  src/downloader.cpp
  src/extractor/generic_extractor.cpp
)

# Warnings
if(MSVC)
  target_compile_options(ffmpeg_downloader PRIVATE /W4)
else()
  target_compile_options(ffmpeg_downloader PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ----------------------------
# Dependencies
# ----------------------------

# CURL
find_package(CURL REQUIRED)
target_link_libraries(ffmpeg_downloader PRIVATE CURL::libcurl)

# CLI11
find_package(CLI11 CONFIG REQUIRED)
target_link_libraries(ffmpeg_downloader PRIVATE CLI11::CLI11)

# nlohmann_json
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(ffmpeg_downloader PRIVATE nlohmann_json::nlohmann_json)

# ----------------------------
# Include folders
# ----------------------------
target_include_directories(ffmpeg_downloader PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ----------------------------
# ffmpeg check
# ----------------------------
find_program(FFMPEG_EXECUTABLE ffmpeg)

if(NOT FFMPEG_EXECUTABLE)
  message(WARNING "ffmpeg not found in PATH. Install with: brew install ffmpeg")
else()
  message(STATUS "Found ffmpeg: ${FFMPEG_EXECUTABLE}")
  target_compile_definitions(ffmpeg_downloader PRIVATE FFMPEG_PATH="${FFMPEG_EXECUTABLE}")
endif()

# ----------------------------
# Install rule
# ----------------------------
install(TARGETS ffmpeg_downloader RUNTIME DESTINATION bin)