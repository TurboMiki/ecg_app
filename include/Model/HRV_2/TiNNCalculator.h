#pragma once
#include <vector>

class TiNNCalculator {
public:
    TiNNCalculator();
    double calculateTiNN(const std::vector<double>& intervals, const std::vector<double>& interval_histogram);

private:
    double TiNN;
};