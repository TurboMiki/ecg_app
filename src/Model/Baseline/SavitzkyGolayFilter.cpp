#include "SavitzkyGolayFilter.h"
#include <gram_savitzky_golay/gram_savitzky_golay.h>

void SavitzkyGolayFilter::set(int FilterLength, int order)
{
    this->filterLength = FilterLength;
    this->order = order;
}

Signal SavitzkyGolayFilter::applyFilter(const Signal& inputSignal) const {

    std::vector<double> oldY = inputSignal.getY;
    std::vector<double> newY;
    int derivative = 0;
    int smoothingPoint = this->filterLength;

    gram_sg::SavitzkyGolayFilter filter(this->filterLength, this->order, smoothingPoint, derivative);

    newY = filter.filter(oldY);

    inputSignal::SetY(newY);
}