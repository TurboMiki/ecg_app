#pragma once
#include "Signal.h"

class Filter {
public:
    virtual ~Filter() = default;

    virtual Signal applyFilter(const Signal& inputSignal) const = 0;
};