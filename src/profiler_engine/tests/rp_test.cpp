#include "data_analyzer.h"
#include <cassert>
#include <vector>
#include <iostream>

int main() {
    DataAnalyzer::RollingPercentile rp(5);
    std::vector<double> v = {1,2,3,4,5,6,7};
    for (double x : v) rp.addSample(x);
    double p50 = rp.getPercentile(0.5);
    double p95 = rp.getPercentile(0.95);
    assert(p50 >= 3 && p50 <= 6);
    assert(p95 >= p50);
    std::cout << "rp_test ok\n";
    return 0;
}

