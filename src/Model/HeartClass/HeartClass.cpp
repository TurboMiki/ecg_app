#include "HeartClass.h"
#include <iostream>
#include <vector>
#include <complex>
#include <fstream>
#include <numeric>
#include <cmath>

/* Constants for readability */
const int R_S_INTERVAL = 60;
const int NUM_OF_INTERVALS = 10;
const int STD_THRESHOLD = 10;
const int QRS_MIN = 50;
const int QRS_MAX = 180;
const int QRS_WIDE_THRESHOLD = 120;

int SamplesToMilliseconds(int value, unsigned int fs) {
    return value * 1000 / fs;
}

enum typeOfActivation HeartActivationClassifier::CheckAVDissociation(const std::vector<int>& P, int currR, const std::vector<int>& QRSonset) {
    std::vector<int> intervals;
    auto itQRS = std::upper_bound(QRSonset.begin(), QRSonset.end(), currR);
    auto itP = std::upper_bound(P.begin(), P.end(), currR);

    for (uint8_t i = 0; i < NUM_OF_INTERVALS; i++) {
        if (itQRS == QRSonset.end() || itP == P.end()) return DIFF_DISEASE;
        intervals.push_back(*itQRS - *itP);
        itQRS++;
        itP++;
    }

    double mean = std::accumulate(intervals.begin(), intervals.end(), 0.0) / intervals.size();
    double variance = std::accumulate(intervals.begin(), intervals.end(), 0.0,
                      [mean](double acc, double x) { return acc + (x - mean) * (x - mean); }) / intervals.size();
    double stdDev = std::sqrt(variance);

    return (stdDev < STD_THRESHOLD) ? DIFF_DISEASE : VENTRICULAR;
}

void HeartActivationClassifier::ClassifyType(const std::vector<int>& rPeaks, const std::vector<int>& P, const std::vector<int>& QRSend, const std::vector<int>& QRSonset, int fs) {
    activations_t nextClassification;

    for (const auto& r : rPeaks) {
        auto itQ = std::lower_bound(QRSonset.begin(), QRSonset.end(), r);
        if (itQ != QRSonset.begin()) {
            itQ--;
        }

        auto itS = std::upper_bound(QRSend.begin(), QRSend.end(), r);

        if (itS != QRSend.end()) {
            int intervalRS = SamplesToMilliseconds(*itS - r, fs);
            int QRS = *itS - *itQ;
            int QRSms = SamplesToMilliseconds(QRS, fs);

            if (QRSms >= QRS_MIN && QRSms <= QRS_MAX) {
                if (QRSms >= QRS_WIDE_THRESHOLD) {
                    if (intervalRS <= R_S_INTERVAL) {
                        nextClassification.actType = CheckAVDissociation(P, r, QRSonset);
                        if (nextClassification.actType == DIFF_DISEASE) {
                            nextClassification.actType = DIFF_DISEASE;
                        } else {
                            nextClassification.actType = VENTRICULAR;
                        }
                    } else {
                        nextClassification.actType = VENTRICULAR;
                    }
                } else {
                    nextClassification.actType = SUPRAVENTRICULAR;
                }
            } else {
                nextClassification.actType = ARTIFACT;
            }

            nextClassification.Ridx = r;
            activations.push_back(nextClassification);
        }
    }
}
