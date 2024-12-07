#include "RRIntervals.h"
#include <cmath>

RRIntervals::RRIntervals(const Signal& rpeaks) {
    calculateIntervals(rpeaks);
}

void RRIntervals::calculateIntervals(const Signal& rpeaks){
        for (size_t i = 0; i < rpeaks.size() - 1; ++i) {
            nnIntervals_.push_back(rpeaks[i + 1] - rpeaks[i]);
        }
}
