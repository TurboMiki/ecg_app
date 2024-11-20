#pragma once
#include "Signal.h"
#include <array>

class HRV_2 {
public:
    void process(const Signal& rIds);
    std::array<float, 4> getParams() const;
    Signal getRHist() const;
    Signal getPointCarePlot() const;

private:
    std::array<float, 4> params_;
    Signal rHist_;
    Signal pointcarePlot_;
};