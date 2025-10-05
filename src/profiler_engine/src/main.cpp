#include "prometheus_client.h"
#include "data_analyzer.h"

#include <iostream>
#include <vector>

/**
 * @brief Entry point for the C++ core profiling engine.
 *
 * This application fetches metrics from Prometheus, parses the JSON results,
 * computes statistics, and optionally detects anomalies. The results are
 * printed to stdout or stored for further visualization.
 *
 * Usage:
 *   ./profiler_engine <prometheus_base_url>
 *
 * Example:
 *   ./profiler_engine http://localhost:9090
 */
int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <prometheus_base_url>" << std::endl;
        return 1;
    }

    std::string prometheusBaseUrl = argv[1];

    // Creates the Prometheus client
    PrometheusClient client(prometheusBaseUrl);

    try {
        // Example queries for our 3 key metrics
        // You can adjust queries based on actual metric names
        std::string latencyQuery   = "avg_over_time(http_request_latency_seconds[1m])";
        std::string throughputQuery = "sum(increase(http_requests_total[1m]))";
        std::string errorRateQuery  = "sum(increase(http_errors_total[1m]))";

        // Queries Prometheus for each metric
        std::string latencyResponse   = client.queryPrometheus(latencyQuery);
        std::string throughputResponse = client.queryPrometheus(throughputQuery);
        std::string errorRateResponse  = client.queryPrometheus(errorRateQuery);

        // Parses the JSON responses
        auto latencyData   = DataAnalyzer::parseMetricJson(latencyResponse, "latency");
        auto throughputData = DataAnalyzer::parseMetricJson(throughputResponse, "throughput");
        auto errorRateData  = DataAnalyzer::parseMetricJson(errorRateResponse, "error_rate");

        // Computes averages
        double avgLatency   = DataAnalyzer::computeAverage(latencyData);
        double avgThroughput = DataAnalyzer::computeAverage(throughputData);
        double avgErrorRate  = DataAnalyzer::computeAverage(errorRateData);

        // Prints results
        std::cout << "Average Latency (last 1m): " << avgLatency << " sec" << std::endl;
        std::cout << "Average Throughput (last 1m): " << avgThroughput << " requests" << std::endl;
        std::cout << "Average Error Rate (last 1m): " << avgErrorRate << " errors" << std::endl;

        // Rolling percentile demo (P50/P95/P99) using synthesized stream as example
        DataAnalyzer::RollingPercentile rp(100);
        for (const auto& d : latencyData) { rp.addSample(d.value); }
        double p50 = rp.getPercentile(0.50);
        double p95 = rp.getPercentile(0.95);
        double p99 = rp.getPercentile(0.99);
        std::cout << "Latency P50/P95/P99: " << p50 << "/" << p95 << "/" << p99 << std::endl;

        // Anomaly detection for latency
        double latencyThreshold = 1.0; // 1 second
        auto latencyAnomalies = DataAnalyzer::detectAnomalies(latencyData, latencyThreshold);
        if (!latencyAnomalies.empty()) {
            std::cout << "Latency anomalies detected on instances: ";
            for (const auto& inst : latencyAnomalies) {
                std::cout << inst << " ";
            }
            std::cout << std::endl;
        } else {
            std::cout << "No latency anomalies detected." << std::endl;
        }

        // Anomaly detection for throughput
        // ...

        // Anomaly detection for error rate
        // ...
        
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception in profiler engine: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}