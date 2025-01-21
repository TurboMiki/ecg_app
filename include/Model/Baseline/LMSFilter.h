#pragma once
#include "Filter.h"

//Class for filtering using LMS Adaptive filtering
class LMSFilter : public Filter {
private:
    //Various parameters needed by filter object from used LMS filtering library
    static const int NUM_TAPS = 32;
    double STEPSIZE = 0.01;
    double REGULARIZATION = 1e-6;
    double inBuffer[NUM_TAPS];
    double weights[NUM_TAPS];

public:
    /*
    LMSFilter()
    Constructor for initialization of filter object with default params
    */
    LMSFilter();
    /*
    Signal applyFilter(const Signal& inputSignal) const override
    Original method used for applying filtering. 
    Depreciated cause of changes in architecture, throws exception and recommends to use adaptiveFilter method instead.
    */
    Signal applyFilter(const Signal& inputSignal) const override;
    /*
    Signal adaptiveFilter(const Signal& inputSignal, const Signal& refSignal)
    Method used for applying adaptive filtering.
    Input arguments:
        - inputSignal: signal that is to be filtered
        - refSignal: reference signal, one that filtering algorithm should assume as ideal and try to achieve it
    */
    Signal adaptiveFilter(const Signal& inputSignal, const Signal& refSignal);
};