#include "Waves.h"
#include <algorithm>
#include <numeric>
#include <stdexcept>

// Konstruktor: Inicjalizuje obiekt Waves z przefiltrowanym sygnałem i listą szczytów R
Waves::Waves(const Signal& filteredSignal, const QList<int>& rPeaks) 
    : signal(filteredSignal) 
{
    // Konwersja QList na std::vector dla przechowywania szczytów R
    rPeaksList.reserve(rPeaks.size());
    for (int peak : rPeaks) {
        rPeaksList.push_back(peak);
    }
}
Waves::Waves(){

}

void Waves::setRPeaks(const QList<int>& rPeaks){
    rPeaksList.reserve(rPeaks.size());
    for (int peak : rPeaks) {
        rPeaksList.push_back(peak);
    }
}

bool Waves::detectWaves(Signal& filteredSignal){
    signal = filteredSignal;
    try {
        if (!detectQRSComplex()) return false;
        if (!detectPWave()) return false;
        if (!detectTWave()) return false;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// Funkcja pomocnicza do obliczania percentyla wartości bezwzględnych w wektorze danych
double Waves::percentileAbs(const std::vector<double>& data, double percentile) {
    // Tworzenie wektora wartości bezwzględnych
    std::vector<double> absData(data.size());
    std::transform(data.begin(), data.end(), absData.begin(), [](double x) { return std::abs(x); });
    
    // Sortowanie wartości bezwzględnych
    std::sort(absData.begin(), absData.end());
    
    // Obliczanie indeksu dla żądanego percentyla
    size_t index = static_cast<size_t>((percentile / 100.0) * absData.size());
    return absData[std::min(index, absData.size() - 1)];
}

// Wykrywa kompleksy QRS, identyfikując ich początki i końce na podstawie gradientów
bool Waves::detectQRSComplex() {
    if (rPeaksList.empty()) {
        return false; // Brak podanych szczytów R
    }

    const auto& y = signal.getY();
    int fs = signal.getSamplingRate();
    int searchWindow = static_cast<int>(0.1 * fs); // Okno przeszukiwania 100ms

    std::vector<int> onsets, ends;

    for (int rPeak : rPeaksList) {
        // Wykrywanie początku QRS
        int searchStart = std::max(0, rPeak - searchWindow);
        int searchEnd = std::min(static_cast<int>(y.size()), rPeak);

        std::vector<double> segment(y.begin() + searchStart, y.begin() + searchEnd);
        std::vector<double> gradients(segment.size() - 1);
        for (size_t i = 0; i < gradients.size(); ++i) {
            gradients[i] = segment[i + 1] - segment[i];
        }

        double maxGradient = *std::max_element(gradients.begin(), gradients.end(),
            [](double a, double b) { return std::abs(a) < std::abs(b); });
        double threshold = std::max(0.1 * std::abs(maxGradient), percentileAbs(gradients, 10));

        // Znajdowanie początku w oknie przeszukiwania
        for (int i = gradients.size() - 1; i >= 0; --i) {
            if (std::abs(gradients[i]) < threshold) {
                onsets.push_back(searchStart + i);
                break;
            }
        }

        // Wykrywanie końca QRS
        searchStart = rPeak;
        searchEnd = std::min(static_cast<int>(y.size()), rPeak + searchWindow);
        segment = std::vector<double>(y.begin() + searchStart, y.begin() + searchEnd);
        gradients.resize(segment.size() - 1);
        for (size_t i = 0; i < gradients.size(); ++i) {
            gradients[i] = segment[i + 1] - segment[i];
        }

        maxGradient = *std::max_element(gradients.begin(), gradients.end(),
            [](double a, double b) { return std::abs(a) < std::abs(b); });
        threshold = std::max(0.05 * std::abs(maxGradient), percentileAbs(gradients, 10));

        // Znajdowanie końca w oknie przeszukiwania
        for (size_t i = 0; i < gradients.size(); ++i) {
            if (std::abs(gradients[i]) < threshold) {
                ends.push_back(searchStart + i);
                break;
            }
        }
    }

    // Przechowywanie wyników jako QVector
    qrsOnsets = QVector<int>(onsets.begin(), onsets.end());
    qrsEnds = QVector<int>(ends.begin(), ends.end());

    return true;
}

// Wykrywa fale P, identyfikując ich początki i końce za pomocą gradientów i analizy amplitudy
bool Waves::detectPWave() {
    if (qrsOnsets.isEmpty()) {
        return false; // Brak wykrytych kompleksów QRS
    }

    const auto& y = signal.getY();
    int fs = signal.getSamplingRate();

    std::vector<int> onsets, ends;

    for (int qrsOnset : qrsOnsets) {
        // Definiowanie okna przeszukiwania dla fali P (200ms przed początkiem QRS)
        int searchWindow = static_cast<int>(0.2 * fs);
        int searchStart = std::max(0, qrsOnset - searchWindow);

        // Identyfikacja szczytu fali P
        std::vector<double> segment(y.begin() + searchStart, y.begin() + qrsOnset);
        auto pPeak = std::max_element(segment.begin(), segment.end());
        int pPeakIdx = searchStart + std::distance(segment.begin(), pPeak);

        // Wykrywanie początku fali P
        searchStart = std::max(0, pPeakIdx - static_cast<int>(0.05 * fs));
        std::vector<double> onsetSegment(y.begin() + searchStart, y.begin() + pPeakIdx - static_cast<int>(0.03 * fs));
        std::vector<double> gradients(onsetSegment.size() - 1);

        for (size_t i = 0; i < gradients.size(); ++i) {
            gradients[i] = onsetSegment[i + 1] - onsetSegment[i];
        }

        if (!gradients.empty()) {
            auto minGradient = std::min_element(gradients.begin(), gradients.end(),
                [](double a, double b) { return std::abs(a) < std::abs(b); });
            onsets.push_back(searchStart + std::distance(gradients.begin(), minGradient));
        }

        // Wykrywanie końca fali P
        searchStart = pPeakIdx + static_cast<int>(0.03 * fs);
        int searchEnd = std::min(static_cast<int>(y.size()), pPeakIdx + static_cast<int>(0.06 * fs));
        std::vector<double> endSegment(y.begin() + searchStart, y.begin() + searchEnd);
        gradients.resize(endSegment.size() - 1);

        for (size_t i = 0; i < gradients.size(); ++i) {
            gradients[i] = endSegment[i + 1] - endSegment[i];
        }

        if (!gradients.empty()) {
            auto minGradient = std::min_element(gradients.begin(), gradients.end(),
                [](double a, double b) { return std::abs(a) < std::abs(b); });
            ends.push_back(searchStart + std::distance(gradients.begin(), minGradient));
        }
    }

    // Przechowywanie wyników jako QVector
    pOnsets = QVector<int>(onsets.begin(), onsets.end());
    pEnds = QVector<int>(ends.begin(), ends.end());

    return true;
}

// Wykrywa końce fali T na podstawie wzrostu amplitudy
bool Waves::detectTWave() {
    if (pOnsets.isEmpty()) {
        return false; // Brak wykrytych fal P
    }

    const auto& y = signal.getY();
    int fs = signal.getSamplingRate();

    std::vector<int> ends;
    int windowSize = 10; // Liczba punktów do analizy wzrostu amplitudy

    for (int pOnset : pOnsets) {
        // Okno przeszukiwania: 50ms przed początkiem każdej fali P
        int searchStart = std::max(0, pOnset - static_cast<int>(0.05 * fs));
        int searchEnd = pOnset;

        if (searchStart >= searchEnd || searchEnd - searchStart < windowSize) continue;

        std::vector<double> segment(y.begin() + searchStart, y.begin() + searchEnd);

        // Identyfikacja maksymalnego wzrostu amplitudy w oknie przeszukiwania
        int maxDiffIdx = -1;
        double maxDiffSum = -std::numeric_limits<double>::infinity();

        for (size_t i = windowSize - 1; i < segment.size(); ++i) {
            double diffSum = 0.0;
            for (size_t j = 0; j < windowSize - 1; ++j) {
                diffSum += segment[i - j] - segment[i - j - 1];
            }

            if (diffSum > maxDiffSum) {
                maxDiffSum = diffSum;
                maxDiffIdx = i;
            }
        }

        // Jeśli znaleziono maksymalny wzrost amplitudy
        if (maxDiffIdx >= windowSize - 1) {
            // Koniec fali T to punkt przed początkiem największego wzrostu
            ends.push_back(searchStart + maxDiffIdx - (windowSize - 1));
        }
    }

    // Przechowywanie wyników jako QVector
    tEnds = QVector<int>(ends.begin(), ends.end());

    return true;
}

// Wykrywa wszystkie fale (QRS, P, T) w sygnale
bool Waves::detectWaves() {
    try {
        if (!detectQRSComplex()) return false; // Wykrywanie kompleksów QRS
        if (!detectPWave()) return false;     // Wykrywanie fal P
        if (!detectTWave()) return false;     // Wykrywanie fal T
        return true;                          // Wszystkie fale wykryte pomyślnie
    } catch (const std::exception&) {
        return false; // Obsługa błędów w przypadku wyjątków
    }
}
