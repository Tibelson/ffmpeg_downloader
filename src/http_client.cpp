#include "http_client.hpp"
#include "config.hpp"
#include "logger.hpp"

#include <curl/curl.h>
#include <stdexcept>

namespace HttpClient {

namespace {

// libcurl write callback — appends received data to a std::string
size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* body = static_cast<std::string*>(userdata);
    body->append(ptr, size * nmemb);
    return size * nmemb;
}

} // anonymous namespace

Response fetch(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize libcurl handle");
    }

    Response resp;
    std::string raw_ct;

    // ----- Options -----
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, Config::USER_AGENT);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, Config::HTTP_MAX_REDIRECTS);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, Config::HTTP_TIMEOUT_SECONDS);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    // Accept compressed responses
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    // Write body
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp.body);

    Logger::debug("HTTP GET → " + url);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::string err = curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        throw std::runtime_error("curl error: " + err);
    }

    // Gather metadata
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp.status_code);

    char* eff_url = nullptr;
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &eff_url);
    if (eff_url) resp.effective_url = eff_url;

    char* ct = nullptr;
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
    if (ct) resp.content_type = ct;

    curl_easy_cleanup(curl);

    resp.ok = (resp.status_code >= 200 && resp.status_code < 300);
    Logger::debug("HTTP " + std::to_string(resp.status_code) +
                  " (" + std::to_string(resp.body.size()) + " bytes)");
    return resp;
}

} // namespace HttpClient
