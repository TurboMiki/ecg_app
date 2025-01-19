#include "RPeaks.h"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <complex>
#include <stdexcept>
#include <fftw3.h> 



/**
 * @brief Default constructor initializing default parameters for R-peak detection.
 */
RPeaks::RPeaks()
    : detection_method("HILBERT"), pan_tompkins_window_length(0), pan_tompkins_threshold(0.0),
      custom_parameters(false), hilbert_proximity(0), hilbert_threshold(0.0) {}



/**
 * @brief Sets parameters for the detection method.
 */
void RPeaks::setParams(const std::string& method, int window_size, double threshold, int proximity) {
    detection_method = method;
    custom_parameters = true;

    if (method == "PAN_TOMPKINS") {
        pan_tompkins_window_length = window_size > 0 ? window_size : static_cast<int>(0.012 * 250); 
        pan_tompkins_threshold = threshold > 0.0 ? threshold : 0.018; 
    } else if (method == "HILBERT") {
        hilbert_proximity = proximity > 0 ? proximity : 100; 
        hilbert_threshold = threshold > 0.0 ? threshold : 1.5; 
    }
}



/**
 * @brief Main function for detecting R-peaks.
 */
bool RPeaks::detectRPeaks(const std::vector<double>& signal, double signal_frequency, std::vector<int>& r_peaks) {
    if (detection_method == "PAN_TOMPKINS") {
        return panTompkins(signal, r_peaks, signal_frequency);
    } else if (detection_method == "HILBERT") {
        return hilbertTransform(signal, r_peaks, signal_frequency);
    }
    return false;
}



/**
 * @brief Computes the FFT.
 */
std::vector<std::complex<double>> computeFFT(const std::vector<std::complex<double>>& input) {
    size_t N = input.size();
    std::vector<std::complex<double>> output(N);

    fftw_complex* in = reinterpret_cast<fftw_complex*>(const_cast<std::complex<double>*>(input.data()));
    fftw_complex* out = reinterpret_cast<fftw_complex*>(output.data());

    fftw_plan plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);

    return output;
}



/**
 * @brief Computes the IFFT.
 */
std::vector<std::complex<double>> computeIFFT(const std::vector<std::complex<double>>& input) {
    size_t N = input.size();
    std::vector<std::complex<double>> output(N);

    fftw_complex* in = reinterpret_cast<fftw_complex*>(const_cast<std::complex<double>*>(input.data()));
    fftw_complex* out = reinterpret_cast<fftw_complex*>(output.data());

    fftw_plan plan = fftw_plan_dft_1d(N, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);

    for (auto& value : output) {
        value /= static_cast<double>(N);
    }

    return output;
}



/**
 * @brief Computes the Hilbert transform of a signal.
 */
std::vector<std::complex<double>> RPeaks::computeHilbert(const std::vector<double>& signal) {
    size_t N = signal.size();
    if (N == 0) {
        throw std::invalid_argument("Signal length must be greater than zero");
    }

    // Conversion of the signal to complex numbers
    std::vector<std::complex<double>> complex_signal(N);
    for (size_t i = 0; i < N; ++i) {
        complex_signal[i] = std::complex<double>(signal[i], 0.0);
    }

    // Computing the FFT of the signal
    auto fft_signal = computeFFT(complex_signal);

    // Creating the Hilbert filter
    std::vector<std::complex<double>> hilbert_filter(N, {0.0, 0.0});
    hilbert_filter[0] = {1.0, 0.0}; // DC component
    for (size_t k = 1; k < (N / 2); ++k) {
        hilbert_filter[k] = {2.0, 0.0}; // Positive frequencies
    }
    if (N % 2 == 0) {
        hilbert_filter[N / 2] = {1.0, 0.0}; // Nyquist frequency
    }

    // Applying the Hilbert filter
    for (size_t k = 0; k < N; ++k) {
        fft_signal[k] *= hilbert_filter[k];
    }

    // Computing the inverse FFT
    return computeIFFT(fft_signal);
}



/**
 * @brief Filters detected peaks based on proximity.
 */
std::vector<int> RPeaks::filterPeaks(const std::vector<int>& peaks, const std::vector<double>& signal, int proximity) {
    std::vector<int> filtered_peaks;
    for (size_t i = 0; i < peaks.size(); ++i) {
        if (filtered_peaks.empty() || (peaks[i] - filtered_peaks.back() > proximity)) {
            filtered_peaks.push_back(peaks[i]);
        } else if (signal[peaks[i]] > signal[filtered_peaks.back()]) {
            filtered_peaks.back() = peaks[i];
        }
    }
    return filtered_peaks;
} 



/**
 * @brief Implements the Pan-Tompkins algorithm.
 */
bool RPeaks::panTompkins(const std::vector<double>& signal, std::vector<int>& r_peaks, double signal_frequency) {
    if (signal.empty()) {
        std::cerr << "Input signal size is 0\n";
        return false;
    }

    // Convolution step: Filter the signal with predefined coefficients
    std::vector<double> filter_coefficients = {-0.125, -0.25, 0.25, 0.125};
    std::vector<double> convolved_signal(signal.size() - filter_coefficients.size() + 1);
    for (size_t i = 0; i < convolved_signal.size(); ++i) {
        convolved_signal[i] = std::inner_product(filter_coefficients.begin(), filter_coefficients.end(),
                                                 signal.begin() + i, 0.0);
    }

    // Squaring step: Square the convolved signal to emphasize larger values
    std::vector<double> squared_signal(convolved_signal.size());
    std::transform(convolved_signal.begin(), convolved_signal.end(), squared_signal.begin(),
                   [](double val) { return val * val; });

    // Moving window integration: Smooth the signal using a moving average
    if (pan_tompkins_window_length == 0) {
        pan_tompkins_window_length = static_cast<int>(0.012 * signal_frequency);
    }
    std::vector<double> integrated_signal(squared_signal.size() - pan_tompkins_window_length + 1);
    for (size_t i = 0; i < integrated_signal.size(); ++i) {
        integrated_signal[i] = std::accumulate(squared_signal.begin() + i,
                                               squared_signal.begin() + i + pan_tompkins_window_length, 0.0);
    }

    
    double max_val = *std::max_element(integrated_signal.begin(), integrated_signal.end());
    double mean_val = std::accumulate(integrated_signal.begin(), integrated_signal.end(), 0.0) / integrated_signal.size();
    if (pan_tompkins_threshold == 0) {
        pan_tompkins_threshold = 0.018; 
    }
   
    // Peak detection: Identify local maxima above the threshold
    std::vector<int> all_peaks; // Tymczasowy wektor na wszystkie załamki
    for (size_t i = 1; i < integrated_signal.size() - 1; ++i) {
        if (integrated_signal[i] > pan_tompkins_threshold && integrated_signal[i] > integrated_signal[i - 1] &&
            integrated_signal[i] > integrated_signal[i + 1]) {
            all_peaks.push_back(i); 
        }
    }

    
    for (size_t i = 1; i < all_peaks.size(); i += 2) {
        r_peaks.push_back(all_peaks[i]);
    }

    return true;


}



/**
 * @brief Implements the Hilbert transform method.
 */
bool RPeaks::hilbertTransform(const std::vector<double>& signal, std::vector<int>& r_peaks, double signal_frequency) {
    if (signal.empty()) {
        std::cerr << "Input signal size is 0\n";
        return false;
    }

    // Hilbert transform: Compute the analytic signal
    auto analytic_signal = computeHilbert(signal);
    // Amplitude envelope: Compute the magnitude of the analytic signal
    std::vector<double> amplitude_envelope(signal.size());
    std::transform(analytic_signal.begin(), analytic_signal.end(), amplitude_envelope.begin(),
                   [](const std::complex<double>& val) { return std::abs(val); });

    // Use the parameters set in setParams
    int proximity = hilbert_proximity;
    double threshold = hilbert_threshold;

    // Peak detection: Identify local maxima above the threshold
    for (size_t i = 1; i < amplitude_envelope.size() - 1; ++i) {
        if (amplitude_envelope[i] > threshold &&
            amplitude_envelope[i] > amplitude_envelope[i - 1] &&
            amplitude_envelope[i] > amplitude_envelope[i + 1]) {
            r_peaks.push_back(i);
        }
    }

    // Filter peaks: Remove duplicates and retain the most prominent peaks
    r_peaks = filterPeaks(r_peaks, signal, proximity);
    return true;
}
