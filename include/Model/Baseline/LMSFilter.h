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
    //Reference signal
    Signal refSignal;

public:
    /*
    LMSFilter()
    Constructor for initialization of filter object with default params
    */
    LMSFilter();
    /*
    void set(const Signal& refSignal)
    Method used to set reference signal
    */
    void set(Signal& refSignal);
    /*
    Signal applyFilter(const Signal& inputSignal) const override
    Method used for applying filtering. 
    */
    Signal applyFilter(const Signal& inputSignal) const override;
    
    /*
    Signal adaptiveFilter(const Signal& inputSignal, const Signal& refSignal)
    **Depreciated**
    Method used for applying adaptive filtering.
    Input arguments:
        - inputSignal: signal that is to be filtered
        - refSignal: reference signal, one that filtering algorithm should assume as ideal and try to achieve it
    */
    //Signal adaptiveFilter(const Signal& inputSignal, const Signal& refSignal);
};