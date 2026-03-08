#pragma once

#include "base_extractor.hpp"
#include "stream_info.hpp"
#include <string>
#include <vector>

/// A fallback extractor that works for any URL.
///
/// Strategy:
///   1. If the URL itself ends with a known media extension → return it directly.
///   2. Otherwise, fetch the page body and run PageAnalyzer on it.
///   3. Results are ordered: HLS first, DASH second, direct files last.
class GenericExtractor : public BaseExtractor {
public:
    /// Supports all URLs (always returns true — used as a fallback).
    bool supports(const std::string& url) const override;

    /// Extract streams from the URL.
    std::vector<StreamInfo> extract(const std::string& url) override;
};
