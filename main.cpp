#include "src/View/MainWindow/mainwindow.h"
#include <QApplication>
#include "Signal.h"
#include "HRV_2.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Wczytanie danych z pliku 100_attr.dat
    std::ifstream file("100_attr.dat");

    if (!file.is_open()) {
        std::cerr << "Nie uda³o siê otworzyæ pliku 100_attr.dat!" << std::endl;
        return -1;
    }

    std::vector<double> signalData;
    std::string line;

    // Plik zawiera dwie kolumny i wczytujemy tylko pierwsz¹
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        double value1, value2;  
        ss >> value1 >> value2; 
        signalData.push_back(value1);
    }

    file.close();

    // Tworzenie obiektu Signal i HRV_2
    Signal signal(signalData);
    HRV_2 hrvProcessor;

    // Przetwarzanie danych
    hrvProcessor.process(signal);

    // Wyœwietlanie danych RRIntervals
    const RRIntervals& rrIntervals = hrvProcessor.getRRIntervals();
    const std::vector<double>& rrData = rrIntervals.getNNIntervals();
    std::cout << "Interwa³y RR (NN):" << std::endl;
    for (const double& interval : rrData) {
        std::cout << interval << " ";
    }
    std::cout << std::endl;

    // Pobranie wyników parametrów
    auto params = hrvProcessor.getParams();
    std::cout << "Triangular Index: " << params[0] << std::endl;
    std::cout << "TiNN: " << params[1] << std::endl;
    std::cout << "SD1: " << params[2] << std::endl;
    std::cout << "SD2: " << params[3] << std::endl;

    // Wyœwietlanie wspó³rzêdnych histogramu
    const auto& histogram = hrvProcessor.getRHist();
    std::cout << "Histogram (rHist) - wspó³rzêdne:" << std::endl;
    double binStart = 0.0;
    double binLength = 0.01;
    for (size_t i = 0; i < histogram.size(); ++i) {
        double binEnd = binStart + binLength;
        std::cout << "x: (" << binStart << " - " << binEnd << "), y: " << histogram[i] << std::endl;
        binStart = binEnd;
    }

    // Wyœwietlanie wspó³rzêdnych wykresu Poincare
    const auto& poincarePlot = hrvProcessor.getPoincarePlot();
    std::cout << "Poincare Plot - wspó³rzêdne (RRn, RRn+1):" << std::endl;
    for (size_t i = 0; i < poincarePlot.size() - 1; ++i) {
        std::cout << "x: " << poincarePlot[i] << ", y: " << poincarePlot[i + 1] << std::endl;
    }

    // Uruchamianie GUI
    MainWindow M;
    M.show();
    return app.exec();
}
