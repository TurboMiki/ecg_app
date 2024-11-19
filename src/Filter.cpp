#include "Filter.h"
#include "Signal.h"
#include <stdexcept>

Filter::Filter() : butterworthOrder(0), butterworthFreq(0.0f), savGolFilterLength(0), savGolOrder(0) {}

Filter::~Filter() {}

void Filter::setButterworthParams(int order, float freq) {
    butterworthOrder = order;
    butterworthFreq = freq;
}

void Filter::setSavGolParams(int filterLength, int order) {
    savGolFilterLength = filterLength;
    savGolOrder = order;
}

Signal Filter::applyButterworth(const Signal& inputSignal) {
    Signal outputSignal;
    return outputSignal;
}

Signal Filter::applySavGol(const Signal& inputSignal) {
    Signal outputSignal;
    return outputSignal;
}

// TODO