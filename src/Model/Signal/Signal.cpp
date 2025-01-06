#define _USE_MATH_DEFINES
#include "Signal.h"
#include <cmath>

Signal Signal::getTestVectors() {
    std::vector<double> x, y;
    int samplingRate = 250;
    int numSamples = 1000;

    for (int i = 0; i < numSamples; ++i) {
        double t = i / static_cast<double>(samplingRate);
        x.push_back(t);
        y.push_back(std::sin(2 * M_PI * 1 * t));
    }

    return Signal(x, y, samplingRate);
}
