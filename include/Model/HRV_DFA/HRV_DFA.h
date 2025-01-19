#pragma once
#include "Signal.h"
#include <array>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <iostream>
using namespace std;

class HRV_DFA {
public:
    void process(const vector <double>& rr_intervals);
    float getA1() const { return a1_; }
    float getA2() const { return a2_; }
    array<vector<double>, 4> getPlotData() const { return {log_vec1_, log_vec2_, log_F1_, log_F2_};
}

private:
    float a1_ = 0.0f;
    float a2_ = 0.0f;
    vector<double> log_vec1_;
    vector<double> log_vec2_;
    vector<double> log_F1_;
    vector<double> log_F2_;

    vector<double> cumulative_sum(const vector<double>& data) const;
    double linear_regression_slope(const vector<double>& x, const vector<double>& y) const;
};