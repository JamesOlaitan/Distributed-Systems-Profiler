#include "data_analyzer.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <cmath>
#include <deque>

using json = nlohmann::json;

std::vector<MetricData> DataAnalyzer::parseMetricJson(const std::string& jsonString,
                                                      const std::string& metricName)
{
    std::vector<MetricData> result;

    try {
        auto parsed = json::parse(jsonString);

        // Prometheus query response format: {"status":"success","data":{"resultType":"vector","result":[...]}}

        // Checks for 'status' and 'data'
        if (parsed.contains("status") && parsed["status"] == "success") {
            if (parsed.contains("data") && parsed["data"].contains("result")) {
                auto results = parsed["data"]["result"];
                for (auto& item : results) {
                    MetricData metricData;
                    if (item.contains("metric") && item["metric"].contains("instance")) {
                        metricData.instance = item["metric"]["instance"].get<std::string>();
                    } else {
                        metricData.instance = "unknown_instance";
                    }

                    if (item.contains("value") && item["value"].is_array()) {
                        auto valueArray = item["value"];
                        // valueArray structure: [timestamp, value]
                        if (valueArray.size() == 2) {
                            metricData.value = std::stod(valueArray[1].get<std::string>());
                        }
                    }
                    result.push_back(metricData);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing " << metricName << " JSON: " << e.what() << std::endl;
    }
    return result;
}

double DataAnalyzer::computeAverage(const std::vector<MetricData>& data)
{
    if (data.empty()) {
        return 0.0;
    }
    double sum = 0.0;
    for (const auto& d : data) {
        sum += d.value;
    }
    return sum / static_cast<double>(data.size());
}

std::vector<std::string> DataAnalyzer::detectAnomalies(const std::vector<MetricData>& data,
                                                       double threshold)
{
    std::vector<std::string> anomalies;
    for (const auto& d : data) {
        if (d.value > threshold) {
            anomalies.push_back(d.instance);
        }
    }
    return anomalies;
}

// --- RollingPercentile implementation ---

DataAnalyzer::RollingPercentile::RollingPercentile(size_t windowSize)
    : m_windowSize(windowSize) {}

void DataAnalyzer::RollingPercentile::addSample(double value)
{
    m_window.push_back(value);
    if (m_lower.empty() || value <= m_lower.top()) {
        m_lower.push(value);
    } else {
        m_upper.push(value);
    }
    rebalance();

    if (m_window.size() > m_windowSize) {
        // Evict oldest; lazy eviction strategy: rebuild heaps when skew detected
        double evicted = m_window.front();
        m_window.pop_front();
        // Rebuild heaps if evicted value equals tops often; to keep code concise, periodically rebuild
        if (m_window.size() % (m_windowSize / 2 + 1) == 0) {
            // Rebuild both heaps from window
            std::priority_queue<double> newLower;
            std::priority_queue<double, std::vector<double>, std::greater<double>> newUpper;
            for (double v : m_window) {
                if (newLower.empty() || v <= newLower.top()) newLower.push(v); else newUpper.push(v);
                // balance
                if (newLower.size() > newUpper.size() + 1) { newUpper.push(newLower.top()); newLower.pop(); }
                else if (newUpper.size() > newLower.size()) { newLower.push(newUpper.top()); newUpper.pop(); }
            }
            m_lower.swap(newLower);
            m_upper.swap(newUpper);
        }
        (void)evicted;
    }
}

double DataAnalyzer::RollingPercentile::getPercentile(double quantile) const
{
    if (m_window.empty()) return 0.0;
    // Copy-heaps approach for simplicity and determinism (O(n log n) overall acceptable for profiling)
    std::vector<double> values(m_window.begin(), m_window.end());
    std::sort(values.begin(), values.end());
    size_t idx = static_cast<size_t>(std::ceil(quantile * values.size())) - 1;
    if (idx >= values.size()) idx = values.size() - 1;
    return values[idx];
}

void DataAnalyzer::RollingPercentile::rebalance()
{
    if (m_lower.size() > m_upper.size() + 1) {
        m_upper.push(m_lower.top());
        m_lower.pop();
    } else if (m_upper.size() > m_lower.size()) {
        m_lower.push(m_upper.top());
        m_upper.pop();
    }
}