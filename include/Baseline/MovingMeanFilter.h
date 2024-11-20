#pragma once
#include "Filter.h"

class MovingMeanFilter : public Filter {
public:
    Signal applyFilter(const Signal& inputSignal) const override;
};