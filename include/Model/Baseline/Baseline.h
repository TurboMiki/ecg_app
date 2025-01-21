#pragma once
#include <memory>
#include <stdexcept>
#include "Signal.h"
#include "Filter.h"

//Baseline Class - for setting and applying chosen filters to acquired signals
class Baseline {

private:
    Signal outSignal;
    std::unique_ptr<Filter> filter;

public:
    Baseline();
    ~Baseline();
    /*
    void setFilter(std::unique_ptr<Filter> newFilter)
    Method for choosing specific filter
    Input arguments:
        newFilter - pointer to chosen and initialized specific filter object (MovingMean, Butterworth, Savitzky-Golay or LMS)
    */
    void setFilter(std::unique_ptr<Filter> newFilter);
    /*
    Signal filterSignal(const Signal& inputSignal)
    Method used to apply filtering to input signal
    Input arguments:
        inputSignal - pointer to input signal that is encapsulated in Signal class
    */
    Signal filterSignal(const Signal& inputSignal);
    /*
    Signal getSignal() const
    Method used to fetch Signal
    */
    Signal getSignal() const;
    /*
    Filter* getFilter()
    Method used to fetch filter
    */
    Filter* getFilter() { return filter.get(); }
};