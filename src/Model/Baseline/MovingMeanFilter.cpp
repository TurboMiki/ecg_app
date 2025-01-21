#include "MovingMeanFilter.h"
#include <cmath>

//Set chosen filter length
void MovingMeanFilter::set(int filterLength) {
    this->filterLength = filterLength;
}

Signal MovingMeanFilter::applyFilter(const Signal& inputSignal) const {
    //Fetch samples of input signal
    const std::vector<double>& oldY = inputSignal.getY();
    
    //Allocate memory for filtered samples
    std::vector<double> newY(oldY.size());
    double sum = 0;

    //Loop for sliding over data
    for(size_t i = 0; i < oldY.size(); i++) {
        //For first and last filterLength/2 samples just rewrite old values
        if(i < std::floor(filterLength/2) || (oldY.size() - i) <= std::floor(filterLength/2)) {
            newY[i] = oldY[i];
        }
        //For further values average values over a specific window
        else {
            sum = 0;
            for(int j = std::floor(i-filterLength/2); j <= std::floor(i+filterLength/2); j++) {
                sum += oldY[j];
            }
            newY[i] = sum/filterLength;
        }
    }
    //Create copy of input signal
    Signal outputSignal = inputSignal;

    //Then change samples for filtered ones
    outputSignal.setY(newY);
    return outputSignal;
}