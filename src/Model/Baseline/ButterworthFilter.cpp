#include "ButterworthFilter.h"
#include <libbutterworth/butterworth.h>
#include <libbutterworth/utils.h>

//Set parameters for filter
void ButterworthFilter::set(int order, float f1, float f2) {
    this->order = order;
    this->freq[0] = f1;
    this->freq[1] = f2;
}

Signal ButterworthFilter::applyFilter(const Signal& inputSignal) const {
    //Fetch samples and sampling rate of input signal
    const std::vector<double>& oldY = inputSignal.getY();
    int samplingRate = inputSignal.getSamplingRate();
    
    //Initialize filter object with specified parameters
    butterworth filter(this->order, this->freq, filter_design::filter_type::bandpass, samplingRate);

    //Allocate memory for vector of filtered samples and proceed with filtering at the same time
    std::vector<double> newY = filter.process(oldY);

    //Create copy of input signal
    Signal outputSignal = inputSignal;

    //Then change samples for filtered ones
    outputSignal.setY(newY);
    return outputSignal;
}