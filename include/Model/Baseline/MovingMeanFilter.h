#pragma once
#include "Filter.h"

//Will take single samples, which will be fed to the filter in loop in main component/program

class MovingMeanFilter : public Filter {
    public:
        MovingMeanFilter(int filterLength);
        double addSample(double newVal);
        double getValue();
        void dumpFilter();

    private:
        double *filterPtr;
        int filterLength;
        double lastVal;

        void initFilter();
        void shiftFilter(double nextVal);
        void computeMean();

};