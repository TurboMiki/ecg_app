#pragma once
#include "Filter.h"

class ButterworthFilter : public Filter {
public:
    Signal applyFilter(const Signal& inputSignal) const override;
};