#include "RPeaks.h"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <complex>
#include <stdexcept>

// Konstruktor klasy
RPeaks::RPeaks()
    : detection_method("HILBERT"), pan_tompkins_window_length(0), pan_tompkins_threshold(0.0), custom_parameters(false) {}

// Ustawienie parametrów detekcji
void RPeaks::setParams(const std::string& method, int window_size, double threshold) {
    detection_method = method;
    pan_tompkins_window_length = window_size;
    pan_tompkins_threshold = threshold;
    custom_parameters = true;
}

// Główna funkcja detekcji załamków R
bool RPeaks::detectRPeaks(const std::vector<double>& signal, double signal_frequency, std::vector<int>& r_peaks) {
    if (detection_method == "PAN_TOMPKINS") {
        return panTompkins(signal, r_peaks, signal_frequency);
    } else if (detection_method == "HILBERT") {
        return hilbertTransform(signal, r_peaks, signal_frequency);
    }
    return false;
}

// =============================
// Obliczenia DFT i IDFT
// =============================

// Obliczanie DFT
std::vector<std::complex<double>> computeDFT(const std::vector<std::complex<double>>& input) {
    size_t N = input.size();
    std::vector<std::complex<double>> output(N);
    const double PI = std::acos(-1);

    for (size_t k = 0; k < N; ++k) {
        std::complex<double> sum(0.0, 0.0);
        for (size_t n = 0; n < N; ++n) {
            double angle = -2.0 * PI * k * n / N;
            sum += input[n] * std::complex<double>(std::cos(angle), std::sin(angle));
        }
        output[k] = sum;
    }

    return output;
}

// Obliczanie IDFT
std::vector<std::complex<double>> computeIDFT(const std::vector<std::complex<double>>& input) {
    size_t N = input.size();
    std::vector<std::complex<double>> output(N);
    const double PI = std::acos(-1);

    for (size_t n = 0; n < N; ++n) {
        std::complex<double> sum(0.0, 0.0);
        for (size_t k = 0; k < N; ++k) {
            double angle = 2.0 * PI * k * n / N;
            sum += input[k] * std::complex<double>(std::cos(angle), std::sin(angle));
        }
        output[n] = sum / static_cast<double>(N);
    }

    return output;
}

// =============================
// Transformacja Hilberta
// =============================

// Implementacja transformacji Hilberta
std::vector<std::complex<double>> RPeaks::computeHilbert(const std::vector<double>& signal) {
    size_t N = signal.size();
    if (N == 0) {
        throw std::invalid_argument("Signal length must be greater than zero");
    }

    // Konwersja sygnału na liczby zespolone
    std::vector<std::complex<double>> complex_signal(N);
    for (size_t i = 0; i < N; ++i) {
        complex_signal[i] = std::complex<double>(signal[i], 0.0);
    }

    // Obliczanie DFT sygnału
    auto fft_signal = computeDFT(complex_signal);

    // Tworzenie filtra Hilberta
    std::vector<std::complex<double>> hilbert_filter(N, {0.0, 0.0});
    hilbert_filter[0] = {1.0, 0.0}; // DC component
    for (size_t k = 1; k < (N / 2); ++k) {
        hilbert_filter[k] = {2.0, 0.0}; // Pozytywne częstotliwości
    }
    if (N % 2 == 0) {
        hilbert_filter[N / 2] = {1.0, 0.0}; // Nyquist frequency
    }

    // Zastosowanie filtra Hilberta
    for (size_t k = 0; k < N; ++k) {
        fft_signal[k] *= hilbert_filter[k];
    }

    // Obliczanie odwrotnej DFT
    return computeIDFT(fft_signal);
}

// =============================
// Filtracja załamków R
// =============================

// Filtracja załamków

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


// =============================
// Metoda Pan-Tompkins
// =============================

// Detekcja załamków R - Pan-Tompkins
bool RPeaks::panTompkins(const std::vector<double>& signal, std::vector<int>& r_peaks, double signal_frequency) {
    if (signal.empty()) {
        std::cerr << "Input signal size is 0\n";
        return false;
    }

    // Konwolucja
    std::vector<double> filter_coefficients = {-0.125, -0.25, 0.25, 0.125};
    std::vector<double> convolved_signal(signal.size() - filter_coefficients.size() + 1);
    for (size_t i = 0; i < convolved_signal.size(); ++i) {
        convolved_signal[i] = std::inner_product(filter_coefficients.begin(), filter_coefficients.end(),
                                                 signal.begin() + i, 0.0);
    }

    // Potęgowanie
    std::vector<double> squared_signal(convolved_signal.size());
    std::transform(convolved_signal.begin(), convolved_signal.end(), squared_signal.begin(),
                   [](double val) { return val * val; });

    // Integracja ruchomego okna
    if (pan_tompkins_window_length == 0) {
        pan_tompkins_window_length = static_cast<int>(0.12 * signal_frequency);
    }
    std::vector<double> integrated_signal(squared_signal.size() - pan_tompkins_window_length + 1);
    for (size_t i = 0; i < integrated_signal.size(); ++i) {
        integrated_signal[i] = std::accumulate(squared_signal.begin() + i,
                                               squared_signal.begin() + i + pan_tompkins_window_length, 0.0);
    }

    // Ustalenie progu
    double max_val = *std::max_element(integrated_signal.begin(), integrated_signal.end());
    double mean_val = std::accumulate(integrated_signal.begin(), integrated_signal.end(), 0.0) / integrated_signal.size();
    double threshold = (0.6 * mean_val + 0.4 * max_val);

    // Detekcja załamków
    for (size_t i = 1; i < integrated_signal.size() - 1; ++i) {
        if (integrated_signal[i] > threshold && integrated_signal[i] > integrated_signal[i - 1] &&
            integrated_signal[i] > integrated_signal[i + 1]) {
            r_peaks.push_back(i);
        }
    }

    return true;
}

// =============================
// Metoda Hilberta
// =============================

// Detekcja załamków R - Hilbert
bool RPeaks::hilbertTransform(const std::vector<double>& signal, std::vector<int>& r_peaks, double signal_frequency) {
    if (signal.empty()) {
        std::cerr << "Input signal size is 0\n";
        return false;
    }

    // Transformacja Hilberta
    auto analytic_signal = computeHilbert(signal);
    std::vector<double> amplitude_envelope(signal.size());
    for (size_t i = 0; i < signal.size(); ++i) {
        amplitude_envelope[i] = std::abs(analytic_signal[i]);
    }

    // Parametry
    int proximity = static_cast<int>(0.15 * signal_frequency);
    //TO DO: Ustawić proximity tak aby wartość zmieniala się dynamicznie w zależności od czestotliwości sygnału
    double threshold = 0.3 * (*std::max_element(amplitude_envelope.begin(), amplitude_envelope.end()));

    // Detekcja załamków
    for (size_t i = 1; i < amplitude_envelope.size() - 1; ++i) {
        if (amplitude_envelope[i] > threshold && amplitude_envelope[i] > amplitude_envelope[i - 1] &&
            amplitude_envelope[i] > amplitude_envelope[i + 1]) {
            r_peaks.push_back(i);
        }
    }

    // Filtracja
    r_peaks = filterPeaks(r_peaks, signal, proximity);
    return true;
}
