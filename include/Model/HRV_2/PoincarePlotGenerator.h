#pragma once
#include "Signal.h"
#include <utility>
#include <vector>

class PoincarePlotGenerator {
public:
    PoincarePlotGenerator();
    void generatePoincarePlot(const std::vector<double>& intervals);
    Signal getPoincarePlot() const;

private:
    std::vector<std::pair<double, double>> poincarePlot_;
};