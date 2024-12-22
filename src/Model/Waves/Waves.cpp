#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <tuple>
#include "Signal.h"
#include <Signal/Signal.h>
#include "RPeaks.h"

// Funkcja do obliczenia percentyla z wartości bezwzględnych
double percentileAbs(const std::vector<double>& data, double percentile) {
    std::vector<double> absData(data.size());
    std::transform(data.begin(), data.end(), absData.begin(), [](double x) { return std::abs(x); });
    std::sort(absData.begin(), absData.end());

    size_t index = static_cast<size_t>((percentile / 100.0) * absData.size());
    return absData[std::min(index, absData.size() - 1)];
}

// Funkcja wykrywająca początek i koniec zespołu QRS
std::pair<std::vector<int>, std::vector<int>> detectQRSOnsetEnd(
    const Signal& signal,
    const std::vector<int>& rPeaks) {
    // rPeaks trzeba będzie zmienić żeby były pobierane z wyjścia modułu Rpeaks, ale teraz mi błędy wychodziły, więc tak zostawiam

    std::vector<int> qrsOnsets;
    std::vector<int> qrsEnds;

    const auto& y = signal.getY();
    int fs = signal.getSamplingRate();
    

    // Obliczanie interwałów RR
    std::vector<double> rrIntervals;
    if (rPeaks.size() > 1) {
        for (size_t i = 1; i < rPeaks.size(); ++i) {
            rrIntervals.push_back((rPeaks[i] - rPeaks[i - 1]) / static_cast<double>(fs));
        }
    } else {
        rrIntervals.push_back(0.6); // Domyślna wartość w przypadku mniejszej liczby R-peaków
    }

    double meanRR = rrIntervals.empty() ? 0.6 : std::accumulate(rrIntervals.begin(), rrIntervals.end(), 0.0) / rrIntervals.size();
    int searchWindow = static_cast<int>(0.1 * meanRR * fs);
    searchWindow = std::clamp(searchWindow, static_cast<int>(0.05 * fs), static_cast<int>(0.2 * fs));

    for (int rPeak : rPeaks) {
        // QRS Onset
        int searchStart = std::max(0, rPeak - searchWindow);
        int searchEnd = rPeak;

        std::vector<double> segment(y.begin() + searchStart, y.begin() + searchEnd);
        std::vector<double> gradients(segment.size() - 1);
        for (size_t i = 0; i < gradients.size(); ++i) {
            gradients[i] = segment[i + 1] - segment[i];
        }

        double maxGradient = *std::max_element(gradients.begin(), gradients.end(), [](double a, double b) { return std::abs(a) < std::abs(b); });
        double threshold = std::max({0.1 * std::abs(maxGradient), percentileAbs(gradients, 10), 0.01});

        for (int i = gradients.size() - 1; i >= 0; --i) {
            if (std::abs(gradients[i]) < threshold) {
                qrsOnsets.push_back(searchStart + i);
                break;
            }
        }

        // QRS End
        searchStart = rPeak;
        searchEnd = std::min(static_cast<int>(y.size()), rPeak + searchWindow * 2);

        segment = std::vector<double>(y.begin() + searchStart, y.begin() + searchEnd);
        gradients.resize(segment.size() - 1);
        for (size_t i = 0; i < gradients.size(); ++i) {
            gradients[i] = segment[i + 1] - segment[i];
        }

        maxGradient = *std::max_element(gradients.begin(), gradients.end(), [](double a, double b) { return std::abs(a) < std::abs(b); });
        threshold = std::max({0.05 * std::abs(maxGradient), percentileAbs(gradients, 10), 0.01});

        for (size_t i = 0; i < gradients.size(); ++i) {
            if (std::abs(gradients[i]) < threshold) {
                qrsEnds.push_back(searchStart + i);
                break;
            }
        }
    }

    return {qrsOnsets, qrsEnds};
}

// Funkcja wykrywająca fale P i ich punkty
std::tuple<std::vector<int>, std::vector<int>, std::vector<int>> detectPWave(
    const Signal& signal,
    const std::vector<int>& qrsOnsets) {

    const auto& y = signal.getY();
    int fs = signal.getSamplingRate();

    std::vector<int> pOnsets;
    std::vector<int> pPeaks;
    std::vector<int> pEnds;

    std::vector<double> qrsIntervals;
    for (size_t i = 1; i < qrsOnsets.size(); ++i) {
        qrsIntervals.push_back((qrsOnsets[i] - qrsOnsets[i - 1]) / static_cast<double>(fs));
    }

    double meanQRSInterval = qrsIntervals.empty() ? 0.6 : std::accumulate(qrsIntervals.begin(), qrsIntervals.end(), 0.0) / qrsIntervals.size();

    int peakWindow = static_cast<int>(0.15 * meanQRSInterval * fs);
    int onsetWindow = static_cast<int>(0.14 * meanQRSInterval * fs);
    int endWindow = static_cast<int>(0.14 * meanQRSInterval * fs);

    for (int qrsOnset : qrsOnsets) {
        // Znajdź szczyt fali P w oknie przed QRS
        int searchStart = std::max(0, qrsOnset - peakWindow);
        int searchEnd = qrsOnset;

        std::vector<double> segment(y.begin() + searchStart, y.begin() + searchEnd);
        auto pPeakLocal = std::max_element(segment.begin(), segment.end());
        int pPeak = searchStart + std::distance(segment.begin(), pPeakLocal);
        pPeaks.push_back(pPeak);

        // Znajdź onset fali P
        searchStart = std::max(0, pPeak - onsetWindow);
        searchEnd = pPeak;

        std::vector<double> onsetSegment(y.begin() + searchStart, y.begin() + searchEnd);
        std::vector<double> onsetGradients(onsetSegment.size() - 1);

        // Oblicz gradienty dla onsetu
        for (size_t i = 0; i < onsetGradients.size(); ++i) {
            onsetGradients[i] = onsetSegment[i + 1] - onsetSegment[i];
        }

        // Znajdź punkt onsetu fali P
        double threshold = 0.01;
        if (!onsetGradients.empty()) {
            auto minOnsetGradient = std::min_element(onsetGradients.begin(), onsetGradients.end(),
                [](double a, double b) { return std::abs(a) < std::abs(b); });
            int pOnset = searchStart + std::distance(onsetGradients.begin(), minOnsetGradient);
            pOnsets.push_back(pOnset);
        }

        // Znajdź koniec fali P
        searchStart = pPeak;
        searchEnd = std::min(static_cast<int>(y.size()), pPeak + endWindow);

        std::vector<double> endSegment(y.begin() + searchStart, y.begin() + searchEnd);
        std::vector<double> endGradients(endSegment.size() - 1);

        // Oblicz gradienty dla końca
        for (size_t i = 0; i < endGradients.size(); ++i) {
            endGradients[i] = endSegment[i + 1] - endSegment[i];
        }

        // Znajdź punkt końca fali P
        if (!endGradients.empty()) {
            auto minEndGradient = std::min_element(endGradients.begin(), endGradients.end(),
                [](double a, double b) { return std::abs(a) < std::abs(b); });
            int pEnd = searchStart + std::distance(endGradients.begin(), minEndGradient);
            pEnds.push_back(pEnd);
        }
    }

    return {pOnsets, pPeaks, pEnds};
}
// Funkcja do wykrywania końca fali T (dziwne punkty wykrywa, więc do poprawy)
std::vector<int> detectTEnds(const Signal& signal, const std::vector<int>& pOnsets) {
    const auto& y = signal.getY();
    int fs = signal.getSamplingRate();

    std::vector<int> tEnds;

    for (int pOnset : pOnsets) {
        int searchStart = pOnset + static_cast<int>(0.2 * fs);
        int searchEnd = std::min(static_cast<int>(y.size()), pOnset + static_cast<int>(0.4 * fs));

        if (searchStart >= searchEnd) {
            continue;
        }

        std::vector<double> segment(y.begin() + searchStart, y.begin() + searchEnd);
        std::vector<double> gradients(segment.size() - 1);
        for (size_t i = 0; i < gradients.size(); ++i) {
            gradients[i] = segment[i + 1] - segment[i];
        }

        double maxGradient = *std::max_element(gradients.begin(), gradients.end(),
            [](double a, double b) { return std::abs(a) < std::abs(b); });
        double threshold = std::max(0.1 * std::abs(maxGradient), percentileAbs(gradients, 10));

        for (size_t i = 0; i < gradients.size(); ++i) {
            if (std::abs(gradients[i]) < threshold && gradients[i] * gradients[i + 1] < 0) {
                tEnds.push_back(searchStart + i);
                break;
            }
        }
    }

    return tEnds;
}