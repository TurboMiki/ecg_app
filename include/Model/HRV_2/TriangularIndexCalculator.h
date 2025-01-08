#pragma once
#include <vector>
#include <iostream>

class TriangularIndexCalculator {
public:
    TriangularIndexCalculator();
    void calculateTriangularIndex(const std::vector<double>& intervals, const IntervalHistogramGenerator& histogramGenerator);
    double getTriangularIndex() const;

private:
    double triangularIndex_;
};