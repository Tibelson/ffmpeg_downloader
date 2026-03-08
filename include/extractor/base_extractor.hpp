#pragma once

#include "stream_info.hpp"
#include <string>
#include <vector>

/// Abstract base class for all site-specific (and generic) extractors.
/// Each subclass knows how to detect and return available media streams
/// from a given URL.
class BaseExtractor {
public:
    virtual ~BaseExtractor() = default;

    /// Return true if this extractor can handle the given URL.
    virtual bool supports(const std::string& url) const = 0;

    /// Fetch and analyze the URL, returning all detected streams.
    /// Throws std::runtime_error on network or parse failure.
    virtual std::vector<StreamInfo> extract(const std::string& url) = 0;
};
