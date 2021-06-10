#ifndef CURL_RAII_H
#define CURL_RAII_H

#include <curl/curl.h>
#include <string>

namespace curl_raii {
struct curl {
private:
    CURL *curl_handle;

public:
    curl();

    curl(const curl &crl) = delete;

    curl &operator=(const curl &crl) = delete;

    curl(curl &&crl) = delete;

    curl &operator=(curl &&crl) = delete;

    void set_url(const std::string &url);

    void switch_verbose(long type);

    void set_progress(long type);

    static void check_error(const CURLcode &res);

    static std::size_t write_data(void *ptr,
                                  size_t size,
                                  size_t nmemb,
                                  void *stream) noexcept;

    void save(std::ostream &output);

    void upload_mode();

    void send();

    void post_mode(const std::string& param);

    ~curl();
};
}  // namespace curl_raii

#endif  // CURL_RAII_H
