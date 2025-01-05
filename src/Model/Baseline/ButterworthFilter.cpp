#include "ButterworthFilter.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <iterator>

#include <libbutterworth/butterworth.h>
#include <libbutterworth/utils.h>

void ButterworthFilter::set(int order, float f1, float f2)
{
    this->order = order;
    this->freq = {f1, f2}
}

Signal ButterworthFilter::applyFilter(const Signal& inputSignal) const {
    
    std::vector<double> oldY = inputSignal.getY;
    std::vector<double> newY;
    SampFreq = inputSignal.getSamplingRate();
    
    butterworth filter(this->order, this->freq, filter_design::filter_type::bandpass, SampFreq);
    
    newY = filter.process(oldY);
    
    inputSignal::SetY(newY);
}