#include "TimeDomainHRV.h"
#include <cmath>


// set intervals, class contructor
TimeDomainHRV::TimeDomainHRV(const Signal& rpeaks) : RRIntervals(rpeaks) {}

// RR_mean calculator
double TimeDomainHRV::calculateRRMean() const{
    double sum = 0.0;

    if (nnIntervals_.empty()){return 0.0;}

    for (double interval : nnIntervals_){
        sum += interval;
    }
    return sum/nnIntervals_.size();
    }


// SDNN caltulator
double TimeDomainHRV::calculateSDNN() const{
    double sum = 0.0;
    double meanRR = calculateRRMean();
    if (nnIntervals_.empty()){return 0.0;}

    for (double interval : nnIntervals_){
        double diff = interval - meanRR;
        sum += diff * diff;
    }

    return std::sqrt(sum/nnIntervals_.size());
}

// RMSSD calculator
double TimeDomainHRV::calculateRMSSD() const{
    double sum = 0.0;
    
    if (nnIntervals_.empty()){return 0.0;}

    for (size_t i = 0; i < nnIntervals_.size() - 1; i++){
        double diff = nnIntervals_[i + 1] - nnIntervals_[i];
        sum += diff * diff;
    }

    return std::sqrt(sum/(nnIntervals_.size() - 1));
}

// NN50 calculator
double TimeDomainHRV::calculateNN50() const{
    int count = 0;

    if (nnIntervals_.empty()){return 0.0;}

    for (size_t i = 0; i < nnIntervals_.size() - 1; i++){
        double diff = nnIntervals_[i + 1] - nnIntervals_[i];
        if (diff > 50){count++;}
    }
    return count;
}

// pNN50 calculator
double TimeDomainHRV::calculatepNN50(double nn50) const{
    if (nn50 == 0.0 || nnIntervals_.empty()){return 0.0;}

    return (nn50/nnIntervals_.size())*100;
    }