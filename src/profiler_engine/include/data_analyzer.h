#ifndef DATA_ANALYZER_H
#define DATA_ANALYZER_H

#include <string>
#include <vector>
#include <map>
#include <queue>

/**
 * @struct MetricData
 * @brief Structure to store metric values fetched from Prometheus.
 */
struct MetricData
{
    std::string instance;
    double value;
};

/**
 * @class DataAnalyzer
 * @brief A class to parse JSON responses, extract metric data,
 *        and perform basic computations like averages and anomaly detection.
 */
class DataAnalyzer
{
public:
    /**
     * @brief Parse the raw JSON string for metric values.
     *
     * @param jsonString The JSON response from Prometheus.
     * @param metricName Name of the metric being parsed (for clarity/logging).
     * @return A vector of MetricData containing instance identifiers and their corresponding values.
     */
    static std::vector<MetricData> parseMetricJson(const std::string& jsonString,
                                                   const std::string& metricName);

    /**
     * @brief Computes the average of the metric values.
     *
     * @param data A vector of MetricData values.
     * @return The average value of the metric.
     */
    static double computeAverage(const std::vector<MetricData>& data);

    /**
     * @brief Detects anomalies based on a threshold approach.
     *
     * @param data A vector of MetricData values.
     * @param threshold The threshold above which a metric is considered anomalous.
     * @return A list of instances whose metric value exceeded the threshold.
     */
    static std::vector<std::string> detectAnomalies(const std::vector<MetricData>& data,
                                                    double threshold);

    /**
     * @brief Rolling percentile (e.g., P50/P95/P99) over a fixed window using two-heaps.
     *
     * Maintains a max-heap for the lower half and min-heap for the upper half,
     * supporting O(log n) insert and lazy eviction, yielding O(n log n) over n events.
     */
    class RollingPercentile {
    public:
        explicit RollingPercentile(size_t windowSize);
        void addSample(double value);
        double getPercentile(double quantile) const; // quantile in (0,1]

    private:
        size_t m_windowSize;
        std::deque<double> m_window;
        std::priority_queue<double> m_lower; // max-heap
        std::priority_queue<double, std::vector<double>, std::greater<double>> m_upper; // min-heap
        void rebalance();
    };
};

#endif // DATA_ANALYZER_H