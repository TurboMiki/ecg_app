#pragma once
#include <vector>

class Signal {
public:
    Signal(const std::vector<double>& x, const std::vector<double>& y, int samplingRate);

    const std::vector<double>& getX() const;
    const std::vector<double>& getY() const;
    int getSize() const;
    int getSamplingRate() const;

    static Signal getTestVectors();

private:
    std::vector<double> x_;
    std::vector<double> y_;
    int N_;
    int samplingRate_;
};