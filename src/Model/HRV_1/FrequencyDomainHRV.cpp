#include "FrequencyDomainHRV.h"
#include <cmath>

// class contructor
 FrequencyDomainHRV::FrequencyDomainHRV(const Signal& ecg) : LombScarglePeriodogram(ecg) {}


  double FrequencyDomainHRV::calculateHF() const{
        double Power = 0.0;
        double Min = 0.15;
        double Max = 0.4;

        for (size_t i = 0; i < frequencies_.size(); i++){
            if(frequencies_[i] >= Min && frequencies_[i] <= Max){
                Power += periodogram_[i] * (frequencies_[i+1] - frequencies_[i]);
            }
        }

 }

  double FrequencyDomainHRV::calculateLF() const{
        double min = 0.04;
        double max = 0.15;

        return calculatePower(min,max);  
 }

  double FrequencyDomainHRV::calculateVLF() const{
        double min = 0.0033;
        double max = 0.04;

        return calculatePower(min,max);   
 }

  double FrequencyDomainHRV::calculateULF() const{
        double min = 0.0;
        double max = 0.0033;   

        return calculatePower(min,max);   
 }

  double FrequencyDomainHRV::calculateTF() const{
        double min = frequencies_.front();
        double max = frequencies_.back();

        return calculatePower(min,max);
 }

  double FrequencyDomainHRV::calculateLFHF() const{
        double lfPower = calculateLF();
        double hfPower = calculateHF();

        if(hfPower != 0){
            return calculateLF()/calculateHF();
        } else{
            return 0;
        }
 }

 double FrequencyDomainHRV::calculatePower(double min, double max) const{
    double power = 0.0;
        for (size_t i = 0; i < frequencies_.size(); i++){
            if(frequencies_[i] >= min && frequencies_[i] <= max){
                power += periodogram_[i] * (frequencies_[i+1] - frequencies_[i]);
            }
        }
    return power;
}

