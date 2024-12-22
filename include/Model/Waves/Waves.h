#pragma once
#include "Signal.h"
#include <Signal/Signal.h>

class Waves {
public:
    void process(const Signal& ecg);
    Signal getPWave() const;
    Signal getQRSWave() const;
    Signal getTWave() const;

private:
    Signal pWave_;
    Signal qrsWave_;
    Signal tWave_;
};