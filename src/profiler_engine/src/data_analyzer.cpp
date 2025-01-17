#include "data_analyzer.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <cmath>

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