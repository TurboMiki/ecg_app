#pragma once
#include <vector>

class FrequencyDomainHRV{
    public:
        // FrequencyDomainHRV(const Signal& ecg);
    protected:
        double calculateTF() const;
        double calculateHF() const;
        double calculateLF() const;
        double calculateVLF() const;
        double calculateULF() const;
        double calculateLFHF() const;
    private:
        // Signal& signal_;
};