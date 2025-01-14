#ifndef PROMETHEUS_CLIENT_H
#define PROMETHEUS_CLIENT_H

#include <string>
#include <vector>
#include <map>

/**
 * @class PrometheusClient
 * @brief An HTTP client to query metrics from Prometheus.
 *
 * This class uses libcurl to perform HTTP GET requests against
 * Prometheus's HTTP API and returns the raw JSON response. It also
 * includes helper methods to parse JSON data for specific metrics.
 */
class PrometheusClient
{
public:
    /**
     * @brief Constructor for PrometheusClient.
     *
     * @param baseUrl The base URL of the Prometheus server (e.g. http://localhost:9090).
     */
    explicit PrometheusClient(const std::string& baseUrl);

    /**
     * @brief Query Prometheus using a given PromQL query.
     *
     * @param promQlQuery The PromQL query string (e.g. "rate(http_requests_total[1m])").
     * @return The raw JSON response as a string.
     */
    std::string queryPrometheus(const std::string& promQlQuery);

private:
    std::string m_baseUrl;  ///< Base URL of the Prometheus server
};

#endif // PROMETHEUS_CLIENT_H