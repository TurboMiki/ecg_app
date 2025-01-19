#include "HRV_DFA.h"
#include <array>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <iostream>
using namespace std;


vector<double> HRV_DFA::cumulative_sum(const vector<double>& data) const {
    vector<double> result(data.size());
    partial_sum(data.begin(), data.end(), result.begin());
    return result;
}

double HRV_DFA::linear_regression_slope(const vector<double>& x, const vector<double>& y) const {
    size_t n = x.size();
    double x_mean = accumulate(x.begin(), x.end(), 0.0) / n;
    double y_mean = accumulate(y.begin(), y.end(), 0.0) / n;

    double num = 0.0, denom = 0.0;
    for (size_t i = 0; i < n; ++i) {
        num += (x[i] - x_mean) * (y[i] - y_mean);
        denom += pow(x[i] - x_mean, 2);
    }
    return num / denom;
}


void HRV_DFA::process(const vector<double>& rr_intervals) {
     if (rr_intervals.empty()) {
        throw runtime_error("Error");
    }
    vector<double> signal = rr_intervals;
    

    size_t len_signal = signal.size();
    vector<int> vec;
    for (int i = 4; i <= 64; ++i) {
        vec.push_back(i);
    }

    double mean_signal = accumulate(signal.begin(), signal.end(), 0.0) / len_signal;
    vector<double> detrended_signal(signal.size());
    for (size_t i = 0; i < signal.size(); ++i) {
        detrended_signal[i] = signal[i] - mean_signal;
    }

    vector<double> y = cumulative_sum(detrended_signal);
    vector<double> x = cumulative_sum(signal);

    vector<double> F;
    for (int n : vec) {
        double s = 0.0;
        size_t num_segments = len_signal / n;

        for (size_t j = 0; j < num_segments; ++j) {
            size_t idx_start = j * n;
            size_t idx_end = idx_start + n;

            vector<double> x_segment(x.begin() + idx_start, x.begin() + idx_end);
            vector<double> y_segment(y.begin() + idx_start, y.begin() + idx_end);

            double slope = linear_regression_slope(x_segment, y_segment);

            double x_mean = accumulate(x_segment.begin(), x_segment.end(), 0.0) / n;
            double y_mean = accumulate(y_segment.begin(), y_segment.end(), 0.0) / n;
            double intercept = y_mean - slope * x_mean;

            vector<double> y_fit(n), y_diff(n);

            for (size_t k = 0; k < n; ++k) {
                y_fit[k] = slope * x_segment[k] + intercept;
                y_diff[k] = y_segment[k] - y_fit[k];
                s += pow(y_diff[k], 2);
            }
        }

        F.push_back(sqrt(s / (num_segments * n)));
    }

    vector<int> vec1(vec.begin(), vec.begin() + 13); 
    vector<int> vec2(vec.begin() + 13, vec.end()); 

    log_vec1_.clear();
    log_vec2_.clear();
    log_F1_.clear();
    log_F2_.clear();

    for (int n : vec1) log_vec1_.push_back(log10(n));
    for (int n : vec2) log_vec2_.push_back(log10(n));

    for (size_t i = 0; i < vec1.size(); ++i) {
        log_F1_.push_back(F[i] > 0 ? log10(F[i]) : 0);
    }

    for (size_t i = vec1.size(); i < F.size(); ++i) {
        log_F2_.push_back(F[i] > 0 ? log10(F[i]) : 0);
    }

    a1_ = static_cast<float>(linear_regression_slope(log_vec1_, log_F1_));
    a2_ = static_cast<float>(linear_regression_slope(log_vec2_, log_F2_));
}