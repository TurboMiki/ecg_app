#pragma once
#include "Signal.h"
#include "TimeDomainHRV.h"
#include "FrequencyDomainHRV.h"
#include <array>
#include <vector>

class HRV_1 : public TimeDomainHRV, public FrequencyDomainHRV{
public:
    HRV_1(const Signal& rpeaks, const Signal& ecg);
    void process();
    std::array<double, 5> getTimeParams() const;
    std::array<double, 6> getFreqParams() const;

private:
    std::array<double, 5> timeParams_;
    std::array<double, 6> freqParams_;
};

