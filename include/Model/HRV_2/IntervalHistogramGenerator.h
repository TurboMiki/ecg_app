#pragma once
#include <vector>

class IntervalHistogramGenerator {
public:
    IntervalHistogramGenerator(double binLength);
    void generateHistogram(const std::vector<double>& intervals);
    const std::vector<int>& getHistogram() const;
    double getHistogramMaxBin() const;

private:
    double binLength_;         
    std::vector<int> histogram_; 
    double histogramMaxBin_;   
};