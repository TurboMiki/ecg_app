#include "HRV_1.h"
#include <cmath>

// class constructor

HRV_1::HRV_1(const Signal& rpeaks, const Signal& ecg) : TimeDomainHRV(rpeaks), FrequencyDomainHRV(signal) {}
// HRV 1 process function
void HRV_1::process(){
    // time parameters
    double meanRR = calculateRRMean();          // 1. RR_mean - rpeaks mean
    double sdnn = calculateSDNN();              //2. SDNN - standard deviation for RR distances
    double rmssd = calculateRMSSD();            //3. RMSSD - root mean sqare from RR intervals distance
    double nn50 = calculateNN50();              //4. NN50  - Pair of neighbours RR with more than 50 distance
    double pnn50 = calculatepNN50(nn50);        //5. pNN50 - percent of NN50 compared to all RR intervals

    timeParams_[0] = meanRR; 
    timeParams_[1] = sdnn;
    timeParams_[2] = rmssd;
    timeParams_[3] = nn50;
    timeParams_[4] = pnn50;

}


// HRV 1 get time parameters
std::array<double, 5> HRV_1::getTimeParams() const{return timeParams_;}

// HRV 1 get frequency paramters
std::array<double, 6>  HRV_1::getFreqParams() const{return freqParams_;}