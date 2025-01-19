#pragma once
#include <Signal.h>
#include <vector>

class RRIntervals{
    public:
        RRIntervals(const Signal& rpeaks);
    protected:
        std::vector<double> nnIntervals_;
    private:
        void calculateIntervals(const Signal& rpeaks);
};