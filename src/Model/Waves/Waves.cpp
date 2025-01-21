#include "Waves.h"
#include <algorithm>
#include <numeric>
#include <stdexcept>

Waves::Waves(const Signal& filteredSignal, const QList<int>& rPeaks) 
    : signal(filteredSignal) 
{
    // Convert QList to std::vector
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

double Waves::percentileAbs(const std::vector<double>& data, double percentile) {
    std::vector<double> absData(data.size());
    std::transform(data.begin(), data.end(), absData.begin(), 
                  [](double x) { return std::abs(x); });
    std::sort(absData.begin(), absData.end());
    
    size_t index = static_cast<size_t>((percentile / 100.0) * absData.size());
    return absData[std::min(index, absData.size() - 1)];
}

bool Waves::detectQRSComplex() {
    if (rPeaksList.empty()) {
        return false;
    }

    const auto& y = signal.getY();
    int fs = signal.getSamplingRate();
    
    // Calculate search window based on sampling rate
    int searchWindow = static_cast<int>(0.1 * fs);  // 100ms window
    
    std::vector<int> onsets, ends;
    
    for (int rPeak : rPeaksList) {
        // QRS Onset detection
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
        
        // Find onset
        for (int i = gradients.size() - 1; i >= 0; --i) {
            if (std::abs(gradients[i]) < threshold) {
                onsets.push_back(searchStart + i);
                break;
            }
        }
        
        // QRS End detection
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
        
        // Find end
        for (size_t i = 0; i < gradients.size(); ++i) {
            if (std::abs(gradients[i]) < threshold) {
                ends.push_back(searchStart + i);
                break;
            }
        }
    }
    
    // Convert to QVector
    qrsOnsets = QVector<int>(onsets.begin(), onsets.end());
    qrsEnds = QVector<int>(ends.begin(), ends.end());
    
    return true;
}

bool Waves::detectPWave() {
    if (qrsOnsets.isEmpty()) {
        return false;
    }

    const auto& y = signal.getY();
    int fs = signal.getSamplingRate();
    
    std::vector<int> onsets, ends;
    
    for (int qrsOnset : qrsOnsets) {
        // Search window for P wave
        int searchWindow = static_cast<int>(0.2 * fs);  // 200ms window
        int searchStart = std::max(0, qrsOnset - searchWindow);
        
        // Find potential P wave peak
        std::vector<double> segment(y.begin() + searchStart, y.begin() + qrsOnset);
        auto pPeak = std::max_element(segment.begin(), segment.end());
        int pPeakIdx = searchStart + std::distance(segment.begin(), pPeak);
        
        // P wave onset
        searchStart = std::max(0, pPeakIdx - static_cast<int>(0.05 * fs));
        std::vector<double> onsetSegment(y.begin() + searchStart, y.begin() + pPeakIdx - static_cast<int>(0.03 * fs)); //zeby uniknac wachan kolo piku
        std::vector<double> gradients(onsetSegment.size() - 1);
        
        for (size_t i = 0; i < gradients.size(); ++i) {
            gradients[i] = onsetSegment[i + 1] - onsetSegment[i];
        }
        
        if (!gradients.empty()) {
            auto minGradient = std::min_element(gradients.begin(), gradients.end(),
                [](double a, double b) { return std::abs(a) < std::abs(b); });
            onsets.push_back(searchStart + std::distance(gradients.begin(), minGradient));
        }
        
        // P wave end
        searchStart = pPeakIdx + static_cast<int>(0.03 * fs); //zeby uniknac wachan kolo piku
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
    
    // Convert to QVector
    pOnsets = QVector<int>(onsets.begin(), onsets.end());
    pEnds = QVector<int>(ends.begin(), ends.end());
    
    return true;
}

bool Waves::detectTWave() {
    if (pOnsets.isEmpty()) {
        return false;
    }

    const auto& y = signal.getY();
    int fs = signal.getSamplingRate();
    
    std::vector<int> ends;
    int windowSize = 10;  // Liczba punktów do analizy wzrostu

    for (int pOnset : pOnsets) {
        // Okno przeszukiwania: od P-onset do P-onset - 0.05 * fs (50 ms przed P-onset)
        int searchStart = std::max(0, pOnset - static_cast<int>(0.05 * fs));  // 50 ms przed P-onset
        int searchEnd = pOnset;  // Do samego P-onset

        if (searchStart >= searchEnd || searchEnd - searchStart < windowSize) continue;

        // Segment do analizy
        std::vector<double> segment(y.begin() + searchStart, y.begin() + searchEnd);

        // Szukamy największego wzrostu w określonej liczbie punktów (windowSize)
        int maxDiffIdx = -1;
        double maxDiffSum = -std::numeric_limits<double>::infinity();

        for (size_t i = windowSize - 1; i < segment.size(); ++i) { 
            double diffSum = 0.0;
            
            // Oblicz sumę różnic dla bieżącego okna o długości `windowSize`
            for (size_t j = 0; j < windowSize - 1; ++j) {
                diffSum += segment[i - j] - segment[i - j - 1];
            }

            if (diffSum > maxDiffSum) {
                maxDiffSum = diffSum;
                maxDiffIdx = i;
            }
        }

        // Jeśli znaleziono największy wzrost amplitudy
        if (maxDiffIdx >= windowSize - 1) {
            // T-fala to punkt przed początkiem największego wzrostu
            ends.push_back(searchStart + maxDiffIdx - (windowSize - 1));
        }
    }

    // Convert to QVector
    tEnds = QVector<int>(ends.begin(), ends.end());

    return true;
}

bool Waves::detectWaves() {
    try {
        if (!detectQRSComplex()) return false;
        if (!detectPWave()) return false;
        if (!detectTWave()) return false;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}
