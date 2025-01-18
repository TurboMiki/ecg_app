#include "HRV_2.h"
#define M_PI 3.14159265358979323846
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>

// Konstruktor
HRV_2::HRV_2() {
    params_.fill(0.0);
}

// Funkcja przetwarzaj�ca sygna� R-peaks
void HRV_2::process(const Signal& rIds) {
    auto rPeaks = rIds.getX();
    int fs = rIds.getSamplingRate();
    if (rPeaks.size() < 2) return;

    // Obliczanie interwa��w RR
    std::vector<double> intervals;
    for (size_t i = 1; i < rPeaks.size(); ++i) {
        intervals.push_back((rPeaks[i] - rPeaks[i - 1]) / static_cast<double>(fs));
    }

    // Generowanie histogramu
    generateHistogram(intervals);

    // Obliczanie TiNN
    calculateTiNN();

    // Obliczanie indeksu tr�jk�tnego
    calculateTriangularIndex(intervals);

    // Tworzenie wykresu Poincare
    generatePoincarePlot(intervals);

    // Dopasowanie elipsy do wykresu Poincare i obliczenie SD1, SD2
    fitPoincareEllipse(intervals);
}

// Generowanie histogramu interwa��w RR
void HRV_2::generateHistogram(const std::vector<double>& intervals) {
    if (intervals.empty()) return;

    // Obliczanie minimalnej i maksymalnej d�ugo�ci interwa�u
    double RRmax = *std::max_element(intervals.begin(), intervals.end());
    double RRmin = *std::min_element(intervals.begin(), intervals.end());

    // Obliczanie d�ugo�ci (rozpi�to��) przedzia�u
    double length = RRmax - RRmin;

    // Obliczanie liczby bin�w (index_hist)
    double HistogramBinLength = 0.01; // D�ugo�� pojedynczego binu (w sekundach), mo�na dostosowa�
    int index_hist = static_cast<int>(length / HistogramBinLength);

    // Obliczanie histogramu
    std::vector<int> histogram(index_hist, 0);
    for (double interval : intervals) {
        int bin = std::min(static_cast<int>((interval - RRmin) / HistogramBinLength), index_hist - 1);
        if (bin >= 0 && bin < index_hist) {
            histogram[bin]++;
        }
    }

    // Przechowanie wynik�w histogramu w odpowiednim formacie
    std::vector<double> histX, histY;
    for (int i = 0; i < histogram.size(); ++i) {
        double binMaxInterval = RRmin + (i + 1) * HistogramBinLength; // Reprezentacja binu jako koniec przedzia�u
        histX.push_back(binMaxInterval);
        histY.push_back(histogram[i]);
    }

    // Znalezienie najliczniejszego binu
    auto maxIt = std::max_element(histogram.begin(), histogram.end());
    int maxBinIndex = std::distance(histogram.begin(), maxIt);

    params_[0] = static_cast<double>(maxBinIndex); // Numer najliczniejszego binu (indeks)
    params_[1] = *maxIt; // maxBinY (liczba wyst�pie� w najliczniejszym binie)

    rHist_ = Signal(histX, histY, 1);
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