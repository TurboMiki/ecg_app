#pragma once
#include "Signal.h"
#include <array>
#include <vector>

class HRV_1 {
public:
    void process(const Signal& rpeaks, const Signal& ecg);
    std::array<double, 5> getTimeParams() const;
    std::array<double, 6> getFreqParams() const;

private:
    std::vector<double> getIntervals(const Signal& rpeaks) const;
    double calculateRRMean(const std::vector<double>& nnIntervals) const;
    double calculateRMSSD(const std::vector<double>& nnIntervals) const;
    double calculateSDNN(const std::vector<double>& nnIntervals) const;
    double calculateNN50(const std::vector<double>& nnIntervals) const;
    double calculatepNN50(const std::vector<double>& nnIntervals, double nn50) const;
    std::array<double, 5> timeParams_;
    std::array<double, 6> freqParams_;
};