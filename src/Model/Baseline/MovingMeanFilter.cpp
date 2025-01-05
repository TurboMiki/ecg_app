#include "MovingMeanFilter.h"
#include <cmath>

void MovingMeanFilter::set(int FilterLength)
{
    this->filterLength=FilterLength;
}

Signal MovingMeanFilter::applyFilter(const Signal& inputSignal) const {

    std::vector<double> oldY = inputSignal.getY;
    std::vector<double> newY;
    double sum = 0;
    for(int i = 0; i<sizeof(oldY);i++)
    {
        if(i<std::floor(filterLength/2) or (sizeof(oldY) - i)<std::floor(filterLength/2)) {
            newY[i] = oldY[i];
        }
        else
        {
            for(int j = std::floor(i-filterLength/2);i<=std::floor(i+filterLength/2);i++)
                sum+=oldY[j];

            newY[i] = sum/filterLength;
            sum = 0;
        }
            
    }
    inputSignal::SetY(newY);
}