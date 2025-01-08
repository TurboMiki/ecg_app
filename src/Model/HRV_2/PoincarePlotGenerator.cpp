#include "PoincarePlotGenerator.h"
#include <iostream>

PoincarePlotGenerator::PoincarePlotGenerator() {}

void PoincarePlotGenerator::generatePoincarePlot(const std::vector<double>& intervals) {
    // Tworzenie wykresu Poincaré 
    poincarePlot_.clear();
    for (size_t i = 0; i < intervals.size() - 1; ++i) {
        poincarePlot_.emplace_back(intervals[i], intervals[i + 1]);
    }
}

Signal PoincarePlotGenerator::getPoincarePlot() const {
    std::vector<double> x, y;
    for (const auto& point : poincarePlot_) {
        x.push_back(point.first);
        y.push_back(point.second);
    }

    return Signal(x, y);
}