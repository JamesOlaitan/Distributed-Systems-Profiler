#include "prometheus_client.h"
#include <curl/curl.h>
#include <stdexcept>
#include <iostream>

/**
 * @brief Writes callback for libcurl to append data to a std::string.
 *
 * @param contents Pointer to the delivered data.
 * @param size Size of a single data chunk (always 1 in text-based data).
 * @param nmemb Number of data chunks.
 * @param userp User-supplied pointer to the string we want to append to.
 * @return The number of bytes processed.
 */
static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    auto* buffer = static_cast<std::string*>(userp);
    buffer->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

PrometheusClient::PrometheusClient(const std::string& baseUrl)
    : m_baseUrl(baseUrl)
{
}

std::string PrometheusClient::queryPrometheus(const std::string& promQlQuery)
{
    // Builds the full URL
    // e.g.: http://localhost:9090/api/v1/query?query=rate(http_requests_total[1m])
    std::string url = m_baseUrl + "/api/v1/query?query=" + promQlQuery;

    CURL* curlHandle = curl_easy_init();
    if (!curlHandle) {
        throw std::runtime_error("Failed to initialize libcurl");
    }

    std::string responseBuffer;
    curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &responseBuffer);

    // Performs request
    CURLcode res = curl_easy_perform(curlHandle);
    if (res != CURLE_OK) {
        curl_easy_cleanup(curlHandle);
        throw std::runtime_error(std::string("CURL error: ") + curl_easy_strerror(res));
    }

    long httpCode(0);
    curl_easy_getinfo(curlHandle, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curlHandle);

    if (httpCode != 200) {
        throw std::runtime_error("HTTP request failed with status code: " + std::to_string(httpCode));
    }

    return responseBuffer;
}