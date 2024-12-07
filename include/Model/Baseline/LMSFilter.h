#pragma once
#include "Filter.h"

class LMSFilter : public Filter {
public:
    Signal applyFilter(const Signal& inputSignal) const override;
};
