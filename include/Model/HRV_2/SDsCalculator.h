#pragma once
#include <vector>

class SDsCalculator {
public:
    SDsCalculator();
    void calculateSDs(const std::vector<double>& intervals);
    double getSD1() const;
    double getSD2() const;

private:
    double SD1_;
    double SD2_;
};