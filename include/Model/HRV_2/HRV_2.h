#pragma once
#include "Signal.h"
#include <array>

class HRV_2 {
public:
    HRV_2();
    void process(const Signal& rIds);
    std::array<double, 4> getParams() const;
    Signal getRHist() const;
    Signal getPoincarePlot() const;

private:
    std::array<double, 4> params_;
    Signal rHist_;
    Signal poincarePlot_;
};