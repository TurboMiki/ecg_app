#pragma once
#include "Filter.h"

class ButterworthFilter : public Filter {
private:
    int order;
    std::vector<double> freq{0.0, 0.0}; // Initialize with default values
public:
    void set(int order, float f1, float f2);
    Signal applyFilter(const Signal& inputSignal) const override;
};