#pragma once
#include "Filter.h"

//Class for filtering using Savitzky-Golay method
class SavitzkyGolayFilter : public Filter {
private:
    int filterLength = 5;
    int order = 3;    
public:
    /*
    void set(int filterLength, int order)
    Method to set length of window for moving mean filtering
    Input arguments:
        - filterLength: length of window in which mean value will be calculated
        - order: order of the filter
    */
    void set(int FilterLength, int order);
    /*
    Signal applyFilter(const Signal& inputSignal) const override
    Method used to apply chosen filtering method
    Input arguments:
        - inputSignal: input signal that is to be filtered
    */
    Signal applyFilter(const Signal& inputSignal) const override;
};
