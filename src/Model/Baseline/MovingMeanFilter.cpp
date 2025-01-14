#include "MovingMeanFilter.h"
#include <cmath>

void MovingMeanFilter::set(int filterLength) {
    this->filterLength = filterLength;
}

Signal MovingMeanFilter::applyFilter(const Signal& inputSignal) const {
    const std::vector<double>& oldY = inputSignal.getY();
    std::vector<double> newY(oldY.size());
    double sum = 0;

    for(size_t i = 0; i < oldY.size(); i++) {
        if(i < std::floor(filterLength/2) || (oldY.size() - i) <= std::floor(filterLength/2)) {
            newY[i] = oldY[i];
        }
        else {
            sum = 0;
            for(int j = std::floor(i-filterLength/2); j <= std::floor(i+filterLength/2); j++) {
                sum += oldY[j];
            }
            newY[i] = sum/filterLength;
        }
    }

    Signal outputSignal = inputSignal; // Create copy
    outputSignal.setY(newY);
    return outputSignal;
}