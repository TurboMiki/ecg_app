#include "SavitzkyGolayFilter.h"
#include <gram_savitzky_golay/gram_savitzky_golay.h>

void SavitzkyGolayFilter::set(int filterLength, int order) {
    this->filterLength = filterLength;
    this->order = order;
}

Signal SavitzkyGolayFilter::applyFilter(const Signal& inputSignal) const {
    const std::vector<double>& oldY = inputSignal.getY();
    int derivative = 0;
    int smoothingPoint = this->filterLength;

    gram_sg::SavitzkyGolayFilter filter(this->filterLength, this->order, smoothingPoint, derivative);
    
    // Create a window to slide over the data
    std::vector<double> newY;
    newY.reserve(oldY.size());
    
    // For the first few points, just copy the original values
    int halfWindow = filterLength / 2;
    for (int i = 0; i < halfWindow; i++) {
        newY.push_back(oldY[i]);
    }
    
    // Apply filter to the middle points
    for (size_t i = halfWindow; i < oldY.size() - halfWindow; i++) {
        // Create a window of points around the current point
        std::vector<double> window(oldY.begin() + i - halfWindow, 
                                 oldY.begin() + i + halfWindow + 1);
        // Apply filter to the window and take the middle point
        newY.push_back(filter.filter(window));
    }
    
    // For the last few points, copy original values
    for (size_t i = oldY.size() - halfWindow; i < oldY.size(); i++) {
        newY.push_back(oldY[i]);
    }

    Signal outputSignal = inputSignal; // Create copy
    outputSignal.setY(newY);
    return outputSignal;
}