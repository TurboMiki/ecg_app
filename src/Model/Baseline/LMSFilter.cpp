#include "LMSFilter.h"
#include "AdaptiveFilter.h"
#include <stdexcept>

LMSFilter::LMSFilter() {
    // Initialize buffers and weights
    for(int i = 0; i < NUM_TAPS; i++) {
        inBuffer[i] = 0.0;
        weights[i] = 0.0;
    }
}

Signal LMSFilter::applyFilter(const Signal& inputSignal) const {
    throw std::runtime_error("LMS Filter requires a reference signal. Use adaptiveFilter instead.");
}

Signal LMSFilter::adaptiveFilter(const Signal& inputSignal, const Signal& refSignal) {
    AfData afData = {
        STEPSIZE,
        REGULARIZATION,
        NUM_TAPS,
        inBuffer,
        0, // initial buffer index
        weights,
        0.0 // initial error
    };
    
    std::vector<double> oldY = inputSignal.getY();
    std::vector<double> refY = refSignal.getY();
    std::vector<double> newY;
    newY.reserve(oldY.size());
    
    for(size_t i = 0; i < oldY.size(); i++) {
        newY.push_back(AdaptiveFilterRun(oldY[i], refY[i], &afData));
    }

    Signal outputSignal = inputSignal; // Create copy
    outputSignal.setY(newY);
    return outputSignal;
}