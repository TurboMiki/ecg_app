#ifndef WAVES_H
#define WAVES_H

#include "Signal.h"
#include <QVector>
#include <vector>

class Waves {
public:
    // Constructor takes filtered signal and R-peaks
    Waves(const Signal& filteredSignal, const QList<int>& rPeaks);
    Waves();
    
    // Process all waves - returns true if successful
    bool detectWaves();
    bool detectWaves(Signal& filteredSignal);
    void setRPeaks(const QList<int>& rPeaks);
    // Getters for wave points
    QVector<int> getQRSOnsets() const { return qrsOnsets; }
    QVector<int> getQRSEnds() const { return qrsEnds; }
    QVector<int> getPOnsets() const { return pOnsets; }
    QVector<int> getPEnds() const { return pEnds; }
    QVector<int> getTEnds() const { return tEnds; }

private:
    // Store input data
    Signal signal;
    std::vector<int> rPeaksList;
    
    // Store results
    QVector<int> qrsOnsets;
    QVector<int> qrsEnds;
    QVector<int> pOnsets;
    QVector<int> pEnds;
    QVector<int> tEnds;
    
    // Helper functions
    double percentileAbs(const std::vector<double>& data, double percentile);
    bool detectQRSComplex();
    bool detectPWave();
    bool detectTWave();
};

#endif // WAVES_H
