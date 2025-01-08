#include "IntervalHistogramGenerator.h"
#include <algorithm>
#include <iostream>

IntervalHistogramGenerator::IntervalHistogramGenerator(double binLength)
    : binLength_(binLength), histogramMaxBin_(0) {}

void IntervalHistogramGenerator::generateHistogram(const std::vector<double>& intervals) {
    // Sprawdzenie, czy wektor interwa³ów nie jest pusty
    if (intervals.empty()) {
        return;
    }

    // Obliczanie minimaln¹ i maksymaln¹ d³ugoœæ interwa³u
    double RRmax = *std::max_element(intervals.begin(), intervals.end());
    double RRmin = *std::min_element(intervals.begin(), intervals.end());

    // Obliczanie d³ugoœci histogramu
    double length = RRmax - RRmin;

    // Obliczanie liczby binów w histogramie
    int numBins = static_cast<int>(length / binLength_);

    // Inicjalizacja histogramu
    histogram_.resize(numBins, 0);

    // Zliczanie interwa³ów w odpowiednich binach
    for (double interval : intervals) {
        int binIndex = static_cast<int>((interval - RRmin) / binLength_);
        if (binIndex >= 0 && binIndex < histogram_.size()) {
            histogram_[binIndex]++;
        }
    }

    // Wyznaczanie najwy¿szego binu
    histogramMaxBin_ = *std::max_element(histogram_.begin(), histogram_.end());
}

const std::vector<int>& IntervalHistogramGenerator::getHistogram() const {
    return histogram_;
}

double IntervalHistogramGenerator::getHistogramMaxBin() const {
    return histogramMaxBin_;
}