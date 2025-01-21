#pragma once
#include "Signal.h"

//Abstract class used for specific filters classes later on
class Filter {
public:
    virtual ~Filter() = default;

    //Placeholder for applying specific filters in other classes
    virtual Signal applyFilter(const Signal& inputSignal) const = 0;
};