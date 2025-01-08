#include "RRIntervals.h"

RRIntervals::RRIntervals(const Signal& rIds) {
    calculateIntervals(rIds);
}

void RRIntervals::calculateIntervals(const Signal& rIds) {
    const std::vector<double>& x = rIds.getX();
    int fs = rIds.getSamplingRate(); // Czêstotliwoœæ próbkowania

    for (size_t i = 1; i < x.size(); ++i) {
        // Przeliczenie interwa³ów na sekundy
        double intervalInSeconds = (x[i] - x[i - 1]) / static_cast<double>(fs);
        nnIntervals_.push_back(intervalInSeconds);
    }
}

const std::vector<double>& RRIntervals::getNNIntervals() const {
    return nnIntervals_;
}

