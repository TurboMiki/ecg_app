#pragma once
#include "Signal.h"
#include "RRIntervals.h"
#include <array>
#include <vector>

class TimeDomainHRV : public RRIntervals {
    TimeDomainHRV(const Signal& rpeaks);
protected:
    double calculateRRMean() const;
    double calculateRMSSD() const;
    double calculateSDNN() const;
    double calculateNN50() const;
    double calculatepNN50(double nn50) const;
};

