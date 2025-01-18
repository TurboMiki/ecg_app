#include "RPeaks.h"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <complex>
#include <stdexcept>
#include <fftw3.h> 

// Konstruktor klasy
RPeaks::RPeaks()
    : detection_method("HILBERT"), pan_tompkins_window_length(0), pan_tompkins_threshold(0.0),
      custom_parameters(false), hilbert_proximity(0), hilbert_threshold(0.0) {}

// Ustawienie parametrów detekcji
void RPeaks::setParams(const std::string& method, int window_size, double threshold, int proximity) {
    detection_method = method;
    custom_parameters = true;

    if (method == "PAN_TOMPKINS") {
        // Ustawienie domyślnych wartości, jeśli użytkownik ich nie podał
        pan_tompkins_window_length = window_size > 0 ? window_size : static_cast<int>(0.012 * 250); // wartość domyślna długości okna
        pan_tompkins_threshold = threshold > 0.0 ? threshold : 3; // wartość domyślna progu detekcji
    } else if (method == "HILBERT") {
        hilbert_proximity = proximity > 0 ? proximity : 5; // wartość domyślna odległości między pikami
        hilbert_threshold = threshold > 0.0 ? threshold : 1.5; // wartość domyślna progu detekcji
    }
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
// Transformacja FFT i IFFT z FFTW
// =============================

// Obliczanie FFT
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

// Obliczanie odwrotnego FFT (IFFT)
std::vector<std::complex<double>> computeIFFT(const std::vector<std::complex<double>>& input) {
    size_t N = input.size();
    std::vector<std::complex<double>> output(N);

    fftw_complex* in = reinterpret_cast<fftw_complex*>(const_cast<std::complex<double>*>(input.data()));
    fftw_complex* out = reinterpret_cast<fftw_complex*>(output.data());

    fftw_plan plan = fftw_plan_dft_1d(N, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);

    // Normalizacja wyników
    for (auto& value : output) {
        value /= static_cast<double>(N);
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

    // Obliczanie FFT sygnału
    auto fft_signal = computeFFT(complex_signal);

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

    // Obliczanie odwrotnej FFT
    return computeIFFT(fft_signal);
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
    std::vector<double> integrated_signal(squared_signal.size() - pan_tompkins_window_length + 1);
    for (size_t i = 0; i < integrated_signal.size(); ++i) {
        integrated_signal[i] = std::accumulate(squared_signal.begin() + i,
                                               squared_signal.begin() + i + pan_tompkins_window_length, 0.0);
    }

    const int refractory_period = static_cast<int>(0.02 * signal_frequency); 

    for (size_t i = 1; i < integrated_signal.size() - 1; ++i) {
        if (integrated_signal[i] > pan_tompkins_threshold &&
            integrated_signal[i] > integrated_signal[i - 1] &&
            integrated_signal[i] > integrated_signal[i + 1]) {
            
            // Sprawdź, czy ostatni wykryty załamek R był wystarczająco dawno
            if (r_peaks.empty() || i - r_peaks.back() > refractory_period) {
                r_peaks.push_back(i);
            }
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
    std::transform(analytic_signal.begin(), analytic_signal.end(), amplitude_envelope.begin(),
                   [](const std::complex<double>& val) { return std::abs(val); });

    // Wartości ustawione w setParams
    int proximity = hilbert_proximity;
    double threshold = hilbert_threshold;

    // Detekcja załamków R
    for (size_t i = 1; i < amplitude_envelope.size() - 1; ++i) {
        if (amplitude_envelope[i] > threshold &&
            amplitude_envelope[i] > amplitude_envelope[i - 1] &&
            amplitude_envelope[i] > amplitude_envelope[i + 1]) {
            r_peaks.push_back(i);
        }
    }

    // Filtracja załamków
    r_peaks = filterPeaks(r_peaks, signal, proximity);
    return true;
}
