#include "TriangularIndexCalculator.h"
#include "IntervalHistogramGenerator.h"
#include <iostream>

TriangularIndexCalculator::TriangularIndexCalculator() : triangularIndex_(0.0) {}

void TriangularIndexCalculator::calculateTriangularIndex(const std::vector<double>& intervals, const IntervalHistogramGenerator& histogramGenerator) {
    if (intervals.empty()) {
        return;
    }

    // Pobieranie histogramu i najwy¿szego binu
    double histogramMaxBin = histogramGenerator.getHistogramMaxBin();

    int totalRRIntervals = intervals.size();

    // Obliczanie indeksu trójk¹tnego
    triangularIndex_ = static_cast<double>(totalRRIntervals) / histogramMaxBin;
}

double TriangularIndexCalculator::getTriangularIndex() const {
    return triangularIndex_;
}