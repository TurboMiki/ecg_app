#include "LMSFilter.h"
#include "../LMS/src/AdaptiveFilter.h"
#include <stdexcept>

//Initialize few parameters needed for this filter
LMSFilter::LMSFilter() {
    //Initialize buffers and weights
    for(int i = 0; i < NUM_TAPS; i++) {
        inBuffer[i] = 0.0;
        weights[i] = 0.0;
    }
}

//Handling errors concerning differences in architecture
Signal LMSFilter::applyFilter(const Signal& inputSignal) const {
    throw std::runtime_error("LMS Filter requires a reference signal. Use adaptiveFilter instead.");
}

//Working filtering method
Signal LMSFilter::adaptiveFilter(const Signal& inputSignal, const Signal& refSignal) {
    //Structure of various parameters specified for filter from LMS library
    AfData afData = {
        STEPSIZE,
        REGULARIZATION,
        NUM_TAPS,
        inBuffer,
        0, // initial buffer index
        weights,
        0.0 // initial error
    };
    
    //Fetching samples of input and reference signals
    std::vector<double> oldY = inputSignal.getY();
    std::vector<double> refY = refSignal.getY();

    //Allocating memory for filtered samples
    std::vector<double> newY;
    newY.reserve(oldY.size());
    
    //Adaptive filtering sample by sample (cause of library written in C and unable to process whole vector at once)
    for(size_t i = 0; i < oldY.size(); i++) {
        newY.push_back(AdaptiveFilterRun(oldY[i], refY[i], &afData));
    }
    
    //Create copy of input signal
    Signal outputSignal = inputSignal; 

    //Change samples for filtered ones
    outputSignal.setY(newY);
    return outputSignal;
}