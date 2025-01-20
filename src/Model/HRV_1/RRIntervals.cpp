#include "RRIntervals.h"
#include <cmath>

RRIntervals::RRIntervals(const Signal& rpeaks) {
    calculateIntervals(rpeaks);
}

void RRIntervals::calculateIntervals(const Signal& rpeaks){
        std::vector<double> x = rpeaks.getX();

        for (size_t i = 0; i < x.size(); i++) {
            x[i] *= 1000; // Unit swap
        }

        for (size_t i = 0; i < x.size() - 1; ++i) {
            nnIntervals_.push_back(x[i + 1] - x[i]);
        }
}
