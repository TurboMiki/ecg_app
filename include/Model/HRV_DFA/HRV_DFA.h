#pragma once
#include "Signal.h"
#include <array>

class HRV_DFA {
public:
    void process(const Signal& rIds);
    float getA1() const;
    float getA2() const;

private:
    float a1_;
    float a2_;
};