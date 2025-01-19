#include "HRV_2.h"
#define M_PI 3.14159265358979323846
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

// Konstruktor
HRV_2::HRV_2() {
    params_.fill(0.0);
}

// Funkcja przetwarzaj�ca sygna� R-peaks
void HRV_2::process(const Signal& rIds) {
    qDebug() << "HRV_2::process - Starting processing";
    auto rPeaks = rIds.getX();
    int fs = rIds.getSamplingRate();
    
    qDebug() << "HRV_2::process - Number of peaks:" << rPeaks.size() 
             << "Sampling rate:" << fs;

    if (rPeaks.size() < 2) {
        qDebug() << "HRV_2::process - Not enough peaks for analysis";
        return;
    }

    // Calculate RR intervals in seconds
    qDebug() << "HRV_2::process - Calculating intervals";
    std::vector<double> intervals;
    for (size_t i = 1; i < rPeaks.size(); ++i) {
        // RR intervals should be in seconds
        double interval = rPeaks[i] - rPeaks[i - 1];
        intervals.push_back(interval);
    }
    
    qDebug() << "HRV_2::process - Calculated" << intervals.size() << "intervals";
    qDebug() << "First few intervals:";
    for (size_t i = 0; i < std::min(size_t(5), intervals.size()); ++i) {
        qDebug() << "  Interval" << i << ":" << intervals[i] << "seconds";
    }

    // Validate intervals
    double meanInterval = 0.0;
    for (double interval : intervals) {
        meanInterval += interval;
    }
    meanInterval /= intervals.size();
    
    qDebug() << "Mean RR interval:" << meanInterval << "seconds";
    
    if (meanInterval < 0.4 || meanInterval > 1.5) {
        qDebug() << "Warning: Mean RR interval outside normal range (0.4-1.5s)";
    }

    try {
        generateHistogram(intervals);
        calculateTiNN();
        calculateTriangularIndex(intervals);
        generatePoincarePlot(intervals);
        fitPoincareEllipse(intervals);
    } catch (const std::exception& e) {
        qDebug() << "HRV_2::process - Exception caught:" << e.what();
        throw;
    }
    
    qDebug() << "HRV_2::process - Processing completed successfully";
}

// Generowanie histogramu interwa��w RR
void HRV_2::generateHistogram(const std::vector<double>& intervals) {
    qDebug() << "generateHistogram - Starting";
    
    if (intervals.empty()) {
        qDebug() << "generateHistogram - Empty intervals vector";
        throw std::runtime_error("No RR intervals available for histogram");
    }

    // Calculate min and max with validation
    auto [minIt, maxIt] = std::minmax_element(intervals.begin(), intervals.end());
    double RRmin = *minIt;
    double RRmax = *maxIt;
    
    qDebug() << "generateHistogram - RR range:" << RRmin << "to" << RRmax 
             << "Number of intervals:" << intervals.size();

    // Validate RR interval values
    if (RRmin < 0 || RRmax <= RRmin) {
        qDebug() << "generateHistogram - Invalid RR interval range";
        throw std::runtime_error("Invalid RR interval range");
    }

    // Calculate histogram parameters
    double length = RRmax - RRmin;
    double binWidth = 0.01; // 10ms bins
    
    // Ensure we have at least 10 bins but not more than 1000
    int numBins = std::max(10, std::min(1000, static_cast<int>(length / binWidth)));
    binWidth = length / numBins; // Recalculate bin width for even distribution
    
    qDebug() << "generateHistogram - Using" << numBins << "bins with width" << binWidth;

    // Create and fill histogram
    std::vector<double> histX(numBins);
    std::vector<double> histY(numBins, 0.0);
    
    // Initialize bin edges
    for (int i = 0; i < numBins; ++i) {
        histX[i] = RRmin + i * binWidth;
    }

    // Fill histogram
    int maxCount = 0;
    int maxBin = 0;
    
    for (double interval : intervals) {
        if (interval >= RRmin && interval <= RRmax) {
            int bin = std::min(numBins - 1, 
                             static_cast<int>((interval - RRmin) / binWidth));
            histY[bin]++;
            
            if (histY[bin] > maxCount) {
                maxCount = histY[bin];
                maxBin = bin;
            }
        }
    }

    qDebug() << "generateHistogram - Histogram filled. Max count:" << maxCount 
             << "at bin:" << maxBin;

    // Store parameters
    params_[0] = maxBin;
    params_[1] = maxCount;

    // Validate histogram data
    if (std::all_of(histY.begin(), histY.end(), [](double v) { return v == 0; })) {
        qDebug() << "generateHistogram - Empty histogram generated";
        throw std::runtime_error("Empty histogram generated");
    }

    // Create Signal object
    try {
        rHist_ = Signal(histX, histY, 1);
        qDebug() << "generateHistogram - Created Signal object with size:" << rHist_.getSize();
        
        if (rHist_.getSize() == 0) {
            throw std::runtime_error("Failed to create histogram Signal object");
        }
    } catch (const std::exception& e) {
        qDebug() << "generateHistogram - Error creating Signal:" << e.what();
        throw;
    }

    qDebug() << "generateHistogram - Successfully completed";
}
// Obliczanie TiNN � znalezienie optymalnego N i M
void HRV_2::calculateTiNN() {
    // Sprawd�, czy histogram interwa��w istnieje
    if (rHist_.getX().empty()) {
        return;
    }

    double accelerator = 0;
    size_t Histogram_size = rHist_.getX().size();
    double globalminimum = std::numeric_limits<double>::infinity();
    bool setglobalminimum = false; // Flaga, kt�ra �ledzi, czy globalne minimum zosta�o ustawione
    double optimalN = 0;
    double optimalM = 0;

    // P�tla przez mo�liwe warto�ci index_N i index_M
    for (double index_N = 0; index_N < Histogram_size - 1; ++index_N) {
        for (double index_M = index_N + 1; index_M < Histogram_size; ++index_M) { // Utw�rz wektor indeks�w
            std::vector<double> x = { rHist_.getX()[static_cast<int>(index_N)], rHist_.getX()[static_cast<int>(index_M)] };

            // Zresetuj akcelerator dla nowych danych
            if (accelerator != 0) {
                accelerator = 0;
            }

            // Interpolation using linear interpolation (without Armadillo)
            std::vector<double> interpolation(Histogram_size, 0);
            double step = (x[1] - x[0]) / (Histogram_size - 1);
            for (size_t i = 0; i < Histogram_size; ++i) {
                interpolation[i] = x[0] + i * step;
            }

            double minimum = 0;

            // Oblicz minimum dla pierwszej cz�ci histogramu
            for (double i = 0; i <= index_N; ++i) {
                double HistogramValue = rHist_.getY()[static_cast<int>(i)];
                minimum += std::pow(HistogramValue, 2);
            }

            // Oblicz minimum po zastosowaniu interpolacji histogramu
            for (size_t i = static_cast<size_t>(index_N + 1); i <= static_cast<size_t>(index_M - 1); ++i) {
                double LinearValue = interpolation[i];
                double HistogramValue = rHist_.getY()[static_cast<int>(i)];
                minimum += std::pow((LinearValue - HistogramValue), 2);
            }

            // Oblicz minimum dla drugiej cz�ci histogramu
            for (double i = index_M; i < Histogram_size; ++i) {
                double HistogramValue = rHist_.getY()[static_cast<int>(i)];
                minimum += std::pow(HistogramValue, 2);
            }

            // Ustaw globalne minimum, je�li nie zosta�o jeszcze ustawione
            if (!setglobalminimum) {
                globalminimum = minimum;
                setglobalminimum = true;
            }

            // Zaktualizuj optymalne warto�ci, je�li bie��ce minimum jest mniejsze lub r�wne globalnemu minimum
            if (minimum <= globalminimum) {
                globalminimum = minimum;
                optimalN = index_N;
                optimalM = index_M;
            }
        }
    }

    // Oblicz TiNN
    double time_N = rHist_.getX()[static_cast<int>(optimalN)];
    double time_M = rHist_.getX()[static_cast<int>(optimalM)];

    params_[2] = time_N;
    params_[3] = time_M;

    // Obliczenie TiNN w milisekundach
    params_[4] = (time_M - time_N) * 1000.0;
}

// Obliczanie indeksu tr�jk�tnego
void HRV_2::calculateTriangularIndex(const std::vector<double>& intervals) {
    if (intervals.empty()) {
        return;
    }

    // Oblicz totalRRIntervals
    size_t totalRRIntervals = intervals.size();

    // Zmienna Y powinna by� r�wna maxBinY (najliczniejszy bin w histogramie)
    double Y = params_[1]; // Pobierz maxBinY z params_[1]

    if (Y != 0) {
        double triangular_index = static_cast<double>(totalRRIntervals) / Y;
        params_[5] = triangular_index;
    }
}

// Tworzenie wykresu Poincare
void HRV_2::generatePoincarePlot(const std::vector<double>& intervals) {
    if (intervals.size() < 2) {
        return;
    }

    // Tworzenie wykresu Poincar�
    std::vector<double> x, y;
    for (size_t i = 0; i < intervals.size() - 1; ++i) {
        x.push_back(intervals[i]);      // Dodaj interwa� RR do osi X
        y.push_back(intervals[i + 1]);  // Dodaj s�siedni interwa� RR do osi Y
    }

    poincarePlot_ = Signal(x, y, 1);
}

// Funkcja dopasowuj�ca elips� do wykresu Poincar� - obliczenie SD1 i SD2
void HRV_2::fitPoincareEllipse(const std::vector<double>& intervals) {
    if (intervals.size() < 2) {
        return;
    }

    // Obliczanie SDSD (standard deviation of successive differences)
    double sdsd = 0.0;
    size_t N = intervals.size();
    for (size_t i = 0; i < N - 1; ++i) {
        double diff = intervals[i + 1] - intervals[i];
        sdsd += diff * diff;
    }
    sdsd = std::sqrt(sdsd / (N - 1));

    // Obliczanie SDNN (standard deviation of all RR intervals)
    double meanRR = std::accumulate(intervals.begin(), intervals.end(), 0.0) / intervals.size();  // �rednia RR
    double sdnn = 0.0;
    for (size_t i = 0; i < N; ++i) {
        double diff = intervals[i] - meanRR;
        sdnn += diff * diff;
    }
    sdnn = std::sqrt(sdnn / N);

    double SD_1 = std::sqrt(0.5) * sdsd;
    double SD_2 = std::sqrt(2 * std::pow(sdnn, 2) - 0.5 * std::pow(sdsd, 2));

    params_[6] = SD_1;
    params_[7] = SD_2;

    // Tworzenie punkt�w elipsy
    std::vector<double> ellipseX, ellipseY;
    std::vector<double> poincareX(intervals.size() - 1), poincareY(intervals.size() - 1);
    for (size_t i = 0; i < intervals.size() - 1; ++i) {
        poincareX[i] = intervals[i];
        poincareY[i] = intervals[i + 1];
    }

    double meanX = std::accumulate(poincareX.begin(), poincareX.end(), 0.0) / poincareX.size();
    double meanY = std::accumulate(poincareY.begin(), poincareY.end(), 0.0) / poincareY.size();

    params_[8] = meanX; // �rodek elipsy - X
    params_[9] = meanY; // �rodek elipsy - Y

    for (double theta = 0; theta < 2 * M_PI; theta += 0.1) {
        // R�wnanie elipsy
        double xEllipse = meanX + SD_2 * std::cos(theta) * std::cos(M_PI / 4) - SD_1 * std::sin(theta) * std::sin(M_PI / 4);
        double yEllipse = meanY + SD_2 * std::cos(theta) * std::sin(M_PI / 4) + SD_1 * std::sin(theta) * std::cos(M_PI / 4);

        ellipseX.push_back(xEllipse);
        ellipseY.push_back(yEllipse);
    }

    // Przechowanie elipsy w obiekcie Signal
    poincareEllipse_ = Signal(ellipseX, ellipseY, 1);
}

// Pobieranie parametr�w analizy HRV
std::array<double, 10> HRV_2::getParams() const {
    return params_;
}

// Pobieranie histogramu interwa��w RR
Signal HRV_2::getRHist() const {
    return rHist_;
}

// Pobieranie wykresu Poincare
Signal HRV_2::getPoincarePlot() const {
    return poincarePlot_;
}

// Pobieranie elipsy Poincare
Signal HRV_2::getPoincareEllipse() const {
    return poincareEllipse_;
}