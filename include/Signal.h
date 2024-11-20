#pragma once
#include <vector>

class Signal {
public:
    Signal()
        : x_{}, y_{}, N_(0), samplingRate_(0) {}

    Signal(const std::vector<double>& x, const std::vector<double>& y, int samplingRate)
        : x_(x), y_(y), N_(x.size()), samplingRate_(samplingRate) {}

    const std::vector<double>& getX() const { return x_; }
    const std::vector<double>& getY() const { return y_; }
    int getSize() const { return N_; }
    int getSamplingRate() const { return samplingRate_; }

    static Signal getTestVectors();

private:
    std::vector<double> x_;
    std::vector<double> y_;
    int N_;
    int samplingRate_;
};