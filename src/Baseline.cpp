#include "Baseline.h"
#include <stdexcept>

Baseline::Baseline() : filterType("") {}

Baseline::~Baseline() {}

void Baseline::setFilterType(const std::string& type) {
    filterType = type;
}

Signal Baseline::filterSignal(const Signal& inputSignal) {
    if (filterType == "Butterworth") {
        outSignal = filter.applyButterworth(inputSignal);
    } else if (filterType == "Savitzky-Golay") {
        outSignal = filter.applySavGol(inputSignal);
    } else {
        throw std::invalid_argument("Unknown filter type");
    }
    return outSignal;
}

Signal Baseline::getSignal() const {
    return outSignal;
}
