#pragma once
#include <string>
#include "Filter.h"

class Baseline {
private:
    Signal outSignal;
    Filter filter;
    std::string filterType;

public:
    Baseline();
    ~Baseline();

    void setFilterType(const std::string& type);

    Signal filterSignal(const Signal& inputSignal);

    Signal getSignal() const;
};
