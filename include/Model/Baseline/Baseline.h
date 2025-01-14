#pragma once
#include <memory>
#include <stdexcept>
#include "Signal.h"
#include "Filter.h"

class Baseline {
private:
    Signal outSignal;
    std::unique_ptr<Filter> filter;

public:
    Baseline();
    ~Baseline();

    void setFilter(std::unique_ptr<Filter> newFilter);
    Signal filterSignal(const Signal& inputSignal);
    Signal getSignal() const;
    Filter* getFilter() { return filter.get(); }
};