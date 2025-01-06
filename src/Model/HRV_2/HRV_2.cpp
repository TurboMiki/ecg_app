#include "HRV_2.h"
#include <algorithm>
#include <cmath>
#include <armadillo>


bool HRV_2::process(const Signal& rIds) {
    if (rIds.getSize() < 2) {
        return false; // Zbyt ma³a liczba punktów R-peaks
    }

    // Krok 1: Generowanie histogramu interwa³ów
    if (!generateIntervalHistogram(rIds)) {
        return false; // B³¹d podczas generowania histogramu
    }

    // Krok 2: Obliczanie TiNN
    if (!calculateTiNN()) {
        return false; // B³¹d podczas obliczania TiNN
    }

    // Krok 3: Obliczanie indeksu trójk¹tnego
    if (!calculateTriangularIndex()) {
        return false; // B³¹d podczas obliczania indeksu trójk¹tnego
    }

    // Krok 4: Generowanie wykresu Poincaré
    if (!generatePoincarePlot()) {
        return false; // B³¹d podczas generowania wykresu
    }

    // Krok 5: Obliczanie SD1 i SD2
    if (!calculateSDs()) {
        return false; // B³¹d podczas obliczania SD1 i SD2
    }

    return true; // Sukces
}

// Generowanie histogramu interwa³ów
bool HRV_2::generateIntervalHistogram(const Signal& rIds) {
    const auto& rPeaks = rIds.getX();
    int samplingRate = rIds.getSamplingRate();
    intervals_.clear();

    // Konwersja punktów R-peaks do interwa³ów czasowych
    for (size_t i = 1; i < rPeaks.size(); ++i) {
        double interval = (rPeaks[i] - rPeaks[i - 1]) / static_cast<double>(samplingRate);
        intervals_.push_back(interval);
    }

    // Znalezienie minimalnych i maksymalnych wartoœci interwa³ów
    double RRmax = *std::max_element(intervals_.begin(), intervals_.end());
    double RRmin = *std::min_element(intervals_.begin(), intervals_.end());
    double binLength = 0.01; // D³ugoœæ binu w sekundach
    int numBins = static_cast<int>((RRmax - RRmin) / binLength) + 1;

    // Tworzenie histogramu zliczaj¹cego interwa³y w ka¿dym binie
    std::vector<double> histogram(numBins, 0.0);
    for (double interval : intervals_) {
        int binIndex = std::min(static_cast<int>((interval - RRmin) / binLength), numBins - 1);
        if (binIndex >= 0 && binIndex < numBins) {
            histogram[binIndex]++;
        }
    }

    // Konwersja histogramu do obiektu Signal
    arma::vec histX = arma::linspace(RRmin, RRmax, numBins);
    arma::vec histY(histogram.begin(), histogram.end());

    rHist_ = Signal(arma::conv_to<std::vector<double>>::from(histX),
        arma::conv_to<std::vector<double>>::from(histY),
        samplingRate);

    // Znalezienie binu z maksymaln¹ wartoœci¹
    arma::uword maxBinIndex;
    double maxBinValue = histY.max(maxBinIndex);
    maxBin_ = histX[maxBinIndex];
    maxBinCount_ = maxBinValue;

    return true;
}

// Obliczanie TiNN
bool HRV_2::calculateTiNN() {
    if (rHist_.getSize() < 3) {
        return false;
    }

    auto histX = rHist_.getX();
    auto histY = rHist_.getY();

    // Konwersja danych histogramu do wektorów Armadillo
    arma::vec x(histX.data(), histX.size(), false);
    arma::vec y(histY.data(), histY.size(), false);

    double globalMin = INFINITY;
    double accelerator = 0.0;
    bool setglobalminimum = false; // Flaga œledz¹ca, czy globalne minimum zosta³o ustawione
    arma::uword optimalN = 0, optimalM = 0; // Indeksy dla optymalnych punktów

    // Przeszukiwanie zakresów histogramu w celu znalezienia minimalnego b³êdu interpolacji
    for (arma::uword N = 0; N < maxBin_ - 1; ++N) {
        for (arma::uword M = maxBin_ + 1; M < x.size(); ++M) {
            // Przygotowanie punktów do interpolacji
            arma::vec interpX = { x[N], maxBin_, x[M] };
            arma::vec interpY = { 0.0, maxBinCount_, 0.0 };

            // Interpolacja liniowa dla bie¿¹cych punktów
            arma::vec interpolated;
            arma::interp1(interpX, interpY, x, interpolated, "linear");

            // Reset akumulatora
            accelerator = 0.0;

            // Czêœæ histogramu przed punktem N
            for (arma::uword i = 0; i <= N; ++i) {
                accelerator += std::pow(y[i], 2);
            }

            // Czêœæ histogramu miêdzy punktami N i M z uwzglêdnieniem interpolacji
            for (arma::uword i = N + 1; i < M; ++i) {
                accelerator += std::pow(interpolated[i] - y[i], 2);
            }

            // Czêœæ histogramu za punktem M
            for (arma::uword i = M; i < x.size(); ++i) {
                accelerator += std::pow(y[i], 2);
            }

            // Ustaw globalne minimum, jeœli nie zosta³o jeszcze ustawione
            if (!setglobalminimum) {
                globalMin = accelerator;
                setglobalminimum = true;
            }

            // Aktualizacja globalnego minimum i optymalnych indeksów
            if (accelerator < globalMin) {
                globalMin = accelerator;
                optimalN = N;
                optimalM = M;
            }
        }
    }

    // Wyznaczenie TiNN na podstawie optymalnych punktów
    TiNN_ = (x[optimalM] - x[optimalN]) * 1000.0; // Wartoœæ w milisekundach
    return true;
}

// Obliczanie indeksu trójk¹tnego
bool HRV_2::calculateTriangularIndex() {
    triangularIndex_ = intervals_.size() / maxBinCount_;
    return true;
}

// Generowanie wykresu Poincaré
bool HRV_2::generatePoincarePlot() {
    if (intervals_.size() < 2) {
        return false;
    }

    std::vector<double> poincareX(intervals_.begin(), intervals_.end() - 1);
    std::vector<double> poincareY(intervals_.begin() + 1, intervals_.end());

    pointcarePlot_ = Signal(poincareX, poincareY, rHist_.getSamplingRate());
    return true;
}

// Obliczanie SD1 i SD2
// Wyznacza parametry SD1 (krótkoterminowe) i SD2 (d³ugoterminowe) na podstawie wykresu Poincaré
bool HRV_2::calculateSDs() {
    if (intervals_.size() < 2) {
        return false;
    }

    arma::vec intervals(intervals_.data(), intervals_.size(), false);
    arma::vec diff = intervals.subvec(1, intervals.n_elem - 1) - intervals.subvec(0, intervals.n_elem - 2);

    // Obliczanie odchylenia standardowego SDSD 
    double SDSD = arma::stddev(diff);

    // Obliczanie odchylenia standardowego NN (SDNN)
    double SDNN = arma::stddev(intervals);

    SD1_ = std::sqrt(0.5) * SDSD;
    SD2_ = std::sqrt(2 * std::pow(SDNN, 2) - 0.5 * std::pow(SDSD, 2));
    
    return true;
}

// Funkcje zwracaj¹ce wyniki
std::array<float, 4> HRV_2::getParams() const {
    return { static_cast<float>(TiNN_), static_cast<float>(triangularIndex_),
            static_cast<float>(SD1_), static_cast<float>(SD2_) };
}

Signal HRV_2::getRHist() const {
    return rHist_;
}

Signal HRV_2::getPoincarePlot() const {
    return pointcarePlot_;
}