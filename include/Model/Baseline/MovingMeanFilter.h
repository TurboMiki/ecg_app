#pragma once
#include "Filter.h"

//Class for filtering using MovingMean method
class MovingMeanFilter : public Filter {
    private:
        int filterLength = 5;
    public:
        /*
        void set(int filterLength)
        Method to set length of window for moving mean filtering
        Input arguments:
            - filterLength: length of window in which mean value will be calculated
        */
        void set(int filterLength);
        /*
        Signal applyFilter(const Signal& inputSignal) const override
        Method used to apply chosen filtering method
        Input arguments:
            - inputSignal: input signal that is to be filtered
        */
        Signal applyFilter(const Signal& inputSignal) const override;
};
