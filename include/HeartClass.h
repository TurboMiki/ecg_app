#pragma once
#include "Signal.h"
#include "HRV_1.h"
#include "Waves.h"

class HeartClass {
public:
    void process(const Signal& p, const Signal& qrs, const Signal& t);
    Signal getHeartClassSignal() const;

private:
    Signal heartClassSignal_;
};