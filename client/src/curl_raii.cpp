#include <curl/curl.h>
#include <curl_raii.h>
#include <iostream>
#include <sstream>
#include <string>

namespace {
std::string construct_error_message(const CURLcode &res) {
    return std::string("libcurl error: ") + curl_easy_strerror(res);
}
}  // namespace

namespace curl_raii {
curl::curl() {
    curl_handle = curl_easy_init();
}

void curl::check_error(const CURLcode &res) {
    if (res != CURLE_OK) {
        std::string message = construct_error_message(res);
        throw std::runtime_error(message);
    }
}

void curl::set_url(const std::string &url) {
    auto res = curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    check_error(res);
}
void curl::switch_verbose(long type) {
    auto res = curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, type);
    check_error(res);
}
void curl::set_progress(long type) {
    auto res = curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, type);
    check_error(res);
}

std::size_t curl::write_data(void *ptr,
                             size_t size,
                             size_t nmemb,
                             void *stream) noexcept {
    static_cast<std::ostream *>(stream)->write(static_cast<char *>(ptr),
                                               size * nmemb);

    return !static_cast<std::ostream *>(stream)->fail() ? size * nmemb : 0;
}

void curl::save(std::ostream &output) {
    auto res = curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
    check_error(res);
    res = curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA,
                           static_cast<void *>(&output));
    check_error(res);
    res = curl_easy_perform(curl_handle);
    check_error(res);
    long response_code = 0;
    res =
        curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
    check_error(res);
    if (response_code != 200) {
        std::string message =
            "Server responded " + std::to_string(response_code);
        if (response_code == 404) {
            message = message + ": page not found";
            throw std::runtime_error(message);
        }
        throw std::runtime_error(message);
    }
}

void curl::upload_mode() {
    auto res = curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "PUT");
    check_error(res);
}

void curl::send() {
    auto res = curl_easy_perform(curl_handle);
    check_error(res);
}

void curl::post_mode(const std::string &param) {
    auto res = curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
    check_error(res);
    res = curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, param.data());
    check_error(res);
}

curl::~curl() {
    curl_easy_cleanup(curl_handle);
}

}  // namespace curl_raii