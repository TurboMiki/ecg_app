#pragma once
#include "Filter.h"

//Class for filtering using Butterworth method
class ButterworthFilter : public Filter {
private:
    int order = 3;
    std::vector<double> freq{1.0, 12.0}; // Initialize with default values
public:
    /*
    void set(int order, float f1, float f2)
    Method used to set filtering parameters (it is assumed that filter type will be band-pass)
    Input arguments:
        - order: filter order
        - f1: lower bound of cut-off frequency
        - f2: upper bound of cut-off frequency
    */
    void set(int order, float f1, float f2);
    /*
    Signal applyFilter(const Signal& inputSignal) const override
    Method used to apply chosen filtering method
    Input arguments:
        - inputSignal: input signal that is to be filtered
    */
    Signal applyFilter(const Signal& inputSignal) const override;
};