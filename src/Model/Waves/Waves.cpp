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
        searchStart = std::max(0, pPeakIdx - static_cast<int>(0.06 * fs));
        std::vector<double> onsetSegment(y.begin() + searchStart, y.begin() + pPeakIdx);
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
        searchStart = pPeakIdx;
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
    if (qrsEnds.isEmpty()) {
        return false;
    }

    const auto& y = signal.getY();
    int fs = signal.getSamplingRate();
    
    std::vector<int> ends;
    
    for (int qrsEnd : qrsEnds) {
        int searchStart = std::min(static_cast<int>(y.size()) - 1, 
                                 qrsEnd + static_cast<int>(0.08 * fs));  // 80ms after QRS
        int searchEnd = std::min(static_cast<int>(y.size()),
                               qrsEnd + static_cast<int>(0.36 * fs));   // 360ms after QRS
        
        if (searchStart >= searchEnd) continue;
        
        std::vector<double> segment(y.begin() + searchStart, y.begin() + searchEnd);
        std::vector<double> gradients(segment.size() - 1);
        
        for (size_t i = 0; i < gradients.size(); ++i) {
            gradients[i] = segment[i + 1] - segment[i];
        }
        
        double maxGradient = *std::max_element(gradients.begin(), gradients.end(),
            [](double a, double b) { return std::abs(a) < std::abs(b); });
        double threshold = std::max(0.1 * std::abs(maxGradient), percentileAbs(gradients, 10));
        
        for (size_t i = 0; i < gradients.size() - 1; ++i) {
            if (std::abs(gradients[i]) < threshold && gradients[i] * gradients[i + 1] < 0) {
                ends.push_back(searchStart + i);
                break;
            }
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