#include "LombScarglePeriodogram.h"
#include <cmath>
#include <iostream>

#define PI 3.141592653589793

LombScarglePeriodogram::LombScarglePeriodogram(const Signal& ecg){
    calculatePeriodogramLS(ecg);
}

void LombScarglePeriodogram::calculatePeriodogramLS(const Signal& ecg){
    
    double omega;
    double C, S, CC, SS, CS;
    double YC, YS, Y, YY;
    double cosOmegaX, sinOmegaX;
    double D;

    const std::vector<double>& x = ecg.getX();
    const std::vector<double>& y = ecg.getY();
    int n = ecg.getSize();
    int samplingRate = ecg.getSamplingRate();
    
    if (x.size() < 2) {
    throw std::runtime_error("Error: ECG signal must contain at least two samples.");
}

    double duration = x.back() - x.front();
    double minFreq = 1.0 / duration;
    double maxFreq = 0.5 * samplingRate;
    size_t nFrequencies = std::max(static_cast<size_t>(500), static_cast<size_t>(samplingRate * duration / 100));

    frequencies_.resize(nFrequencies);
    periodogram_.resize(nFrequencies);

    for (size_t i = 0; i < nFrequencies; i++) {
        frequencies_[i] = minFreq + i * (maxFreq - minFreq) / (nFrequencies - 1);
    }

    for (size_t i = 0; i < nFrequencies; i++){
        omega = 2.0 * PI * frequencies_[i];
        C = 0.0, S = 0.0, CC = 0.0, SS = 0.0, CS = 0.0;
        YC = 0.0, YS = 0.0, Y = 0.0, YY = 0.0;

        for (size_t j = 0; j < n; j++){
            cosOmegaX = cos(omega * x[j]);
            sinOmegaX = sin(omega * x[j]);

            C += cosOmegaX;
            S += sinOmegaX;
            CC += cosOmegaX * cosOmegaX;
            SS += sinOmegaX * sinOmegaX;
            CS += cosOmegaX * sinOmegaX;

            Y += y[j];
            YY += y[j] * y[j];
            YC += y[j] * cosOmegaX;
            YS += y[j] * sinOmegaX;
        }
        D = CC * SS - CS * CS;
        
        if (abs(D) < 1e-10){
            periodogram_[i] = 0;
            continue;
        }
        else{
        periodogram_[i] = (SS * YC * YC + CC * YS * YS - 2.0 * CS * YC * YS) / (YY * D);
        }
    }
    for (size_t i = 0; i < periodogram_.size(); i++) {
    std::cout << "Periodogram[" << i << "] = " << periodogram_[i] << std::endl;
}

}
