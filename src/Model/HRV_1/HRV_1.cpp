#include "HRV_1.h"
#include <cmath>

// HRV 1 process function
void HRV_1::process(const Signal& rpeaks, const Signal& ecg){
    // variables
    double meanRR = 0.0;
    double sdnn = 0.0;
    double rmssd = 0.0;
    double nn50 = 0.0;
    double pnn50 = 0.0;

    // time parameters
    std::vector<double> nnIntervals = getIntervals(rpeaks);
    meanRR = calculateRRMean(nnIntervals);          // 1. RR_mean - rpeaks mean
    sdnn = calculateSDNN(nnIntervals);              //2. SDNN - standard deviation for RR distances
    rmssd = calculateRMSSD(nnIntervals);            //3. RMSSD - root mean sqare from RR intervals distance
    nn50 = calculateNN50(nnIntervals);              //4. NN50  - Pair of neighbours RR with more than 50 distance
    pnn50 = calculatepNN50(nnIntervals, nn50);      //5. pNN50 - percent of NN50 compared to all RR intervals

    timeParams_[0] = meanRR; 
    timeParams_[1] = sdnn;
    timeParams_[2] = rmssd;
    timeParams_[3] = nn50;
    timeParams_[4] = pnn50;

}

// get intervals from Rpeaks
std::vector<double> HRV_1::getIntervals(const Signal& rpeaks) const{
        std::vector<double> nnIntervals;
        for (size_t i = 0; i < rpeaks.size() - 1; i++){
        nnIntervals.push_back(rpeaks[i + 1] - rpeaks[i]);
    }
    return nnIntervals;
}

// RR_mean calculator
double HRV_1::calculateRRMean(const std::vector<double>& nnIntervals) const{
    double sum = 0.0;

    for (double interval : nnIntervals){
        sum += interval;
    }
    return sum/nnIntervals.size();
    }


// SDNN caltulator
double HRV_1::calculateSDNN(const std::vector<double>& nnIntervals) const{
    double sum = 0.0;
    double meanRR = calculateRRMean(nnIntervals);
    if (nnIntervals.empty()){return 0.0;}

    for (double interval : nnIntervals){
        double diff = interval - meanRR;
        sum += diff * diff;
    }

    return std::sqrt(sum/nnIntervals.size());
}

 std::array<float, 5> timeParams_;
// RMSSD calculator
double HRV_1::calculateRMSSD(const std::vector<double>& nnIntervals) const{
    double sum = 0.0;
    
    if (nnIntervals.empty()){return 0.0;}

    for (size_t i = 0; i < nnIntervals.size() - 1; i++){
        double diff = nnIntervals[i + 1] - nnIntervals[i];
        sum += diff * diff;
    }

    return std::sqrt(sum/(nnIntervals.size() - 1));
}

// NN50 calculator
double HRV_1::calculateNN50(const std::vector<double>& nnIntervals) const{
    int count = 0;

    if (nnIntervals.empty()){return 0.0;}

    for (size_t i = 0; i < nnIntervals.size() - 1; i++){
        double diff = nnIntervals[i + 1] - nnIntervals[i];
        if (diff > 50){count++;}
    }
    return count;
}

// pNN50 calculator
double HRV_1::calculatepNN50(const std::vector<double>& nnIntervals, double nn50) const{return (nn50/nnIntervals.size())*100;}

// HRV 1 getting time parameters
std::array<double, 5> HRV_1::getTimeParams() const{return timeParams_;}

// HRV 1 getting frequency paramters
std::array<double, 6>  HRV_1::getFreqParams() const{return freqParams_;}