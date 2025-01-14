#pragma once
#include "Filter.h"

class LMSFilter : public Filter {
private:
    static const int NUM_TAPS = 32;
    double STEPSIZE = 0.01;
    double REGULARIZATION = 1e-6;
    double inBuffer[NUM_TAPS];
    double weights[NUM_TAPS];

public:
    LMSFilter();
    Signal applyFilter(const Signal& inputSignal) const override;
    Signal adaptiveFilter(const Signal& inputSignal, const Signal& refSignal);
};