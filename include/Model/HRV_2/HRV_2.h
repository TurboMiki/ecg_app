#pragma once
#include <array>
#include "Signal.h"

class HRV_2 {
public:
    HRV_2();

    void process(const Signal& rIds);
    void generateHistogram(const std::vector<double>& intervals);
    void calculateTiNN();
    void calculateTriangularIndex(const std::vector<double>& intervals);
    void generatePoincarePlot(const std::vector<double>& intervals);
    void fitPoincareEllipse(const std::vector<double>& intervals);

    std::array<double, 10> getParams() const;
    Signal getRHist() const;
    Signal getPoincarePlot() const;
    Signal getPoincareEllipse() const;

private:
    std::array<double, 10> params_;
    Signal rHist_;
    Signal poincarePlot_;
    Signal poincareEllipse_;
};