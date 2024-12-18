#include "MovingMeanFilter.h"

MovingMeanFilter::MovingMeanFilter(int filterLength)
{
    this->filterLength = filterLength;
    this->filterPtr = new double[filterLength];
    this->lastVal = 0.0;
    initFilter();
}

double MovingMeanFilter::addSample(double newVal)
{
    shiftFilter(newVal);
    computeMean();
    return this->lastVal;
}

double MovingMeanFilter::getValue()
{
    return this->lastVal;
}

void MovingMeanFilter::dumpFilter()
{
    this->lastVal = 0.0;
    initFilter();
}

void MovingMeanFilter::shiftFilter(double nextVal)
{
    for(int i = this->filterLength - 1; i > -1; i--)
    {
        if(i==0)
            *(filterPtr) = nextVal;
        else
            *(filterPtr+i) = *(filterPtr + (i-1));
    }
}

void MovingMeanFilter::computeMean()
{
    double sum = 0.0;
    for(int i = 0; i< this->filterLength; i++)
    {
        sum+= *(filterPtr+1);
    }
    this->lastVal = sum / this->filterLength;
}

void MovingMeanFilter::initFilter()
{
    for(int i = 0; i < this->filterLength; i++)
    {
        *(filterPtr+i) = 0.0;
    }
}