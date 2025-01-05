#pragma once
#include "Filter.h"

class MovingMeanFilter : public Filter {
    private:
        int filterLength = 5;
    public:
        void set(int filterLength);
        Signal applyFilter(const Signal& inputSignal) const override;
};
