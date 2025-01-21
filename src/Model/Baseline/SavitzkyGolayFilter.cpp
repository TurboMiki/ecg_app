#include "SavitzkyGolayFilter.h"
#include <gram_savitzky_golay/gram_savitzky_golay.h>


//Set parameters for filter
void SavitzkyGolayFilter::set(int filterLength, int order) {
    this->filterLength = filterLength;
    this->order = order;
}

Signal SavitzkyGolayFilter::applyFilter(const Signal& inputSignal) const {
    //Fetch samples of input signal
    const std::vector<double>& oldY = inputSignal.getY();
    
    //Specify a few more parameters for gram_sg filter object
    int derivative = 0;
    int smoothingPoint = this->filterLength;

    //Initialize filter with given parameters
    gram_sg::SavitzkyGolayFilter filter(this->filterLength, smoothingPoint, this->order, derivative);
    
    //Allocate memory for filtered samples
    std::vector<double> newY;
    newY.reserve(oldY.size());
    
    //For the first few points, just copy the original values
    int halfWindow = this->filterLength;
    for (int i = 0; i < halfWindow; i++) {
        newY.push_back(oldY[i]);
    }
    
    //Apply filter to the middle points
    for (size_t i = halfWindow; i < oldY.size() - halfWindow; i++) {
        //Create a window of points around the current point
        std::vector<double> window(oldY.begin() + i - halfWindow, 
                                 oldY.begin() + i + halfWindow + 1);
        //Apply filter to the window and take the middle point
        newY.push_back(filter.filter(window));
    }
    
    //For the last few points, copy original values
    for (size_t i = oldY.size() - halfWindow; i < oldY.size(); i++) {
        newY.push_back(oldY[i]);
    }

    //Create copy of input signal
    Signal outputSignal = inputSignal;
    
    //Then change samples for filtered ones
    outputSignal.setY(newY);
    return outputSignal;
}