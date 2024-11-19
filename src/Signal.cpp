#include "Signal.h"
#include <cmath>

Signal Signal::getTestVectors() {
    std::vector<double> x, y;
    int samplingRate = 250;
    for (int i = 0; i < 1000; ++i) {
        x.push_back(i / static_cast<double>(samplingRate));
        y.push_back(sin(2 * M_PI * 1 * x.back()));
    }
    return Signal(x, y, samplingRate);
}