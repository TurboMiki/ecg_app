#pragma once
#include "Signal.h"
#include <vector>

class RRIntervals {
public:
    RRIntervals(const Signal& rIds);
    const std::vector<double>& getNNIntervals() const;

private:
    void calculateIntervals(const Signal& rIds);
    std::vector<double> nnIntervals_;
};