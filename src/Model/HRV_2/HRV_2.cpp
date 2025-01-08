#include "HRV_2.h"
#include "RRIntervals.h"
#include "IntervalHistogramGenerator.h"
#include "TiNNCalculator.h"
#include "TriangularIndexCalculator.h"
#include "PoincarePlotGenerator.h"
#include "SDsCalculator.h"
#include <iostream>

HRV_2::HRV_2()
    : params_({ 0.0, 0.0, 0.0, 0.0 }), rHist_(), pointcarePlot_() {}

void HRV_2::process(const Signal& rIds) {
    // Generowanie interwa³ów NN
    RRIntervals rrIntervals(rIds);
    const std::vector<double>& intervals = rrIntervals.getNNIntervals();  // Pobieramy interwa³y NN

    if (intervals.empty()) {
        std::cerr << "Brak interwa³ów do przetworzenia!" << std::endl;
        return;
    }

    // Generowanie histogramu interwa³ów
    IntervalHistogramGenerator histogramGenerator(0.01);  // Przyk³adowa d³ugoœæ binu 0.01
    histogramGenerator.generateHistogram(intervals);
    rHist_ = histogramGenerator.getHistogram();  // Przypisanie wygenerowanego histogramu

    // Wyznaczanie najwy¿szego binu (histogramMaxBin)
    double histogramMaxBin = histogramGenerator.getHistogramMaxBin();

    // Obliczanie TiNN
    TiNNCalculator tinCalculator;
    double TiNN = tinCalculator.calculateTiNN(intervals, rHist_);

    // Obliczanie indeksu trójk¹tnego
    TriangularIndexCalculator triangularIndexCalculator;
    double triangularIndex = triangularIndexCalculator.calculateTriangularIndex(intervals, histogramMaxBin);

    // Tworzenie wykresu Poincare
    PoincarePlotGenerator poincareGenerator;
    poincareGenerator.generatePoincarePlot(intervals);
    pointcarePlot_ = poincareGenerator.getPoincarePlot();  // Przypisanie wygenerowanego wykresu

    // Obliczanie SD1 i SD2
    SDsCalculator sdsCalculator;
    sdsCalculator.calculateSDs(intervals);
    double SD1 = sdsCalculator.getSD1();
    double SD2 = sdsCalculator.getSD2();

    // Przypisywanie wyników do params_
    params_[0] = triangularIndex;
    params_[1] = TiNN;
    params_[2] = SD1;
    params_[3] = SD2;
}

std::array<double, 4> HRV_2::getParams() const {
    return params_;
}

Signal HRV_2::getRHist() const {
    return rHist_;
}

Signal HRV_2::getPoincarePlot() const {
    return pointcarePlot_;
}