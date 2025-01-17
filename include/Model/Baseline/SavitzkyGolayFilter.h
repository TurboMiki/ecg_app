#pragma once
#include "Filter.h"

class SavitzkyGolayFilter : public Filter {
private:
    int filterLength;
    int order;    
public:
    void set(int FilterLength, int order);
    Signal applyFilter(const Signal& inputSignal) const override;
};
