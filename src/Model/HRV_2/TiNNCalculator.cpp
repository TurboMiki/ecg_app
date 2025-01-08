#include "TiNNCalculator.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <armadillo>

double TiNNCalculator::calculateTiNN(const std::vector<double>& intervals, const std::vector<double>& interval_histogram) {
    // Sprawdzanie, czy wektor interval_histogram i intervals nie s¹ puste
    if (intervals.empty() || interval_histogram.empty()) {
        return 0.0;
    }

    // Inicjalizacja zmiennych
    double accelerator = 0;
    size_t histogram_size = interval_histogram.size();
    double globalminimum = INFINITY;
    bool setglobalminimum = false;  // Flaga œledz¹ca, czy globalne minimum zosta³o ustawione
    double optimalN = 0;
    double optimalM = 0;

    // Pêtla przez mo¿liwe wartoœci index_N
    for (double index_N = 0; index_N < (intervals.size() - 1); ++index_N) {
        for (double index_M = (intervals.size() + 1); index_M < histogram_size; ++index_M) {
            std::vector<double> x = { index_N, intervals.size(), index_M };  // Utwórz wektor indeksów

            // Zresetowanie akceleratora dla nowych danych
            if (accelerator != 0) {
                accelerator = 0;
            }

            // Zainicjowana interpolacja (arma::interp1 do interpolacji)
            arma::vec x_values = { x[0], x[1], x[2] };
            arma::vec y_values = { 0, static_cast<double>(interval_histogram.size()), 0 };

            // Interpolacja liniowa
            arma::vec interpolation;
            arma::interp1(x_values, y_values, arma::linspace<arma::vec>(x_values[0], x_values[2], histogram_size), interpolation, "linear");

            double minimum = 0;

            // Obliczanie minimum dla pierwszej czêœci histogramu
            for (double i = 0; i <= index_N; ++i) {
                double HistogramValue = static_cast<double>(interval_histogram[i]);
                minimum += std::pow(HistogramValue, 2);
            }

            // Obliczanie minimum po zastosowaniu interpolacji histogramu
            for (arma::uword i = static_cast<arma::uword>(index_N + 1); i <= static_cast<arma::uword>(index_M - 1); ++i) {
                double LinearValue = interpolation.at(i);
                double HistogramValue = static_cast<double>(interval_histogram[i]);
                minimum += std::pow((LinearValue - HistogramValue), 2);
            }

            // Obliczanie minimum dla drugiej czêœci histogramu
            for (double i = index_M; i < histogram_size; ++i) {
                double HistogramValue = static_cast<double>(interval_histogram[i]);
                minimum += std::pow(HistogramValue, 2);
            }

            // Ustawianie globalnego minimum
            if (!setglobalminimum) {
                globalminimum = minimum;
                setglobalminimum = true;
            }

            // Zaktualizowanie optymalnych wartoœci, jeœli bie¿¹ce minimum jest mniejsze lub równe globalnemu minimum
            if (minimum <= globalminimum) {
                globalminimum = minimum;
                optimalN = index_N;
                optimalM = index_M;
            }
        }
    }

    // Obliczanie TiNN
    double time_N = interval_histogram[optimalN];
    double time_M = interval_histogram[optimalM];
    std::cout << "time_M: " << time_M << std::endl;
    std::cout << "time_N: " << time_N << std::endl;

    double TiNN = (time_M - time_N) * 1000.0;  // Wyra¿any w milisekundach
    return TiNN;
}