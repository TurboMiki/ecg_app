#pragma once
#include "Filter.h"

class SavitzkyGolayFilter : public Filter {
public:
    Signal applyFilter(const Signal& inputSignal) const override;
};
