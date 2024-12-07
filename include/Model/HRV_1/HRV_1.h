#pragma once
#include "Signal.h"
#include <array>
#include <vector>

class HRV_1 {
public:
    void process(const Signal& rpeaks, const Signal& ecg);
    std::array<float, 5> getTimeParams() const;
    std::array<float, 6> getFreqParams() const;

private:
    double calculateRMSSD(const std::vector<double>& nnIntervals) const;
    double calculateSDNN(const std::vector<double>& nnIntervals) const;
    std::array<float, 5> timeParams_;
    std::array<float, 6> freqParams_;
};