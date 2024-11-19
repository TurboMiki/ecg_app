#pragma once
#include "Signal.h"

class RPeaks {
public:
    void process(const Signal& ecg, int method);
    Signal getRpeaks() const;

private:
    void panTompkins(const Signal& ecg);
    void hilbertTransform(const Signal& ecg);

    Signal rpeaks_;
};