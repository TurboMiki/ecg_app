#include "ButterworthFilter.h"
#include <libbutterworth/butterworth.h>
#include <libbutterworth/utils.h>

void ButterworthFilter::set(int order, float f1, float f2) {
    this->order = order;
    this->freq[0] = f1;
    this->freq[1] = f2;
}

Signal ButterworthFilter::applyFilter(const Signal& inputSignal) const {
    const std::vector<double>& oldY = inputSignal.getY();
    int samplingRate = inputSignal.getSamplingRate();
    
    butterworth filter(this->order, this->freq, filter_design::filter_type::bandpass, samplingRate);
    std::vector<double> newY = filter.process(oldY);
    
    Signal outputSignal = inputSignal; // Create copy
    outputSignal.setY(newY);
    return outputSignal;
}