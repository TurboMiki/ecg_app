#pragma once
#include "LombScarglePeriodogram.h"
#include <vector>

class FrequencyDomainHRV : public LombScarglePeriodogram{
    public:
         FrequencyDomainHRV(const Signal& ecg);
    protected:
        double calculateHF() const;
        double calculateLF() const;
        double calculateVLF() const;
        double calculateULF() const;
        double calculateTF() const;
        double calculateLFHF() const;
        double calculatePower(double min, double max) const;
};