#include "HRV_2.h"
#include "Signal.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <iomanip>

// Funkcja do wczytania danych z pliku i utworzenia obiektu Signal
Signal loadSignalFromFile(const std::string& filename, int samplingRate) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Nie uda³o siê otworzyæ pliku: " << filename << std::endl;
        return Signal(); // Zwraca pusty obiekt Signal w przypadku b³êdu
    }

    std::vector<double> signalDataX;
    std::string line;

    // Wczytywanie tylko pierwszej kolumny
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        double value1;
        ss >> value1; // Wczytujemy tylko pierwsz¹ kolumnê
        signalDataX.push_back(value1);
    }
    file.close();

    return Signal(signalDataX, {}, samplingRate); // Druga kolumna jest zbêdna, wiêc przekazujemy pusty wektor
}


int main() {
    // Wczytanie sygna³u z pliku "100_attr.dat"
    std::string filename = "C:/Users/ja/Desktop/HRV_2/tests/100_attr.dat";
    int samplingRate = 360; // Przyjmujemy, ¿e czêstotliwoœæ próbkowania wynosi 100 Hz

    Signal signal = loadSignalFromFile(filename, samplingRate);
    if (signal.getSize() < 2) {
        std::cerr << "Za ma³o danych w pliku do przeprowadzenia analizy." << std::endl;
        return -1;
    }

    // Tworzenie obiektu klasy HRV_2
    HRV_2 hrv;
    hrv.process(signal);

    std::cout << "===== Wyniki parametrow dla HRV_2 =====" << std::endl;

    // Pobieranie obliczonych parametrów
    std::array<double, 10> params = hrv.getParams();

    // Wyœwietlenie wyników
    std::cout << "Najliczniejszy bin (index): " << params[0] << std::endl;
    std::cout << "Liczba wystapien interwalow RR w najliczniejszym binie: " << params[1] << std::endl;
    std::cout << "Optymalny N [s]: " << params[2] << std::endl;
    std::cout << "Optymalny M [s]: " << params[3] << std::endl;
    std::cout << "TiNN [ms]: " << params[4] << std::endl;
    std::cout << "Triangular Index: " << params[5] << std::endl;
    std::cout << "SD1: " << params[6] << std::endl;
    std::cout << "SD2: " << params[7] << std::endl;
    std::cout << "Srodek elipsy - X: " << params[8] << std::endl;
    std::cout << "Srodek elipsy - Y: " << params[9] << std::endl;

    std::cout << "\n===== Histogram interwalow RR =====" << std::endl;

    // Wyœwietlanie wartoœci dla histogramu
    Signal rHist = hrv.getRHist();
    std::cout << "\nPunkty histogramu:" << std::endl;
    for (size_t i = 0; i < rHist.getX().size(); ++i) {
        std::cout << std::fixed << std::setprecision(6) << rHist.getX()[i] << " " << rHist.getY()[i] << std::endl;
    }

    std::cout << "\n===== Wykres Poincare =====" << std::endl;

    // Wyœwietlanie wartoœci dla wykresu Poincare
    Signal poincarePlot = hrv.getPoincarePlot();
    std::cout << "\nPunkty wykresu Poincare:" << std::endl;
    for (size_t i = 0; i < poincarePlot.getX().size(); ++i) {
        std::cout << std::fixed << std::setprecision(6) << poincarePlot.getX()[i] << " " << poincarePlot.getY()[i] << std::endl;
    }

    std::cout << "\n===== Elipsa Poincare =====" << std::endl;

    // Wyœwietlanie wartoœci dla elipsy Poincare
    Signal poincareEllipse = hrv.getPoincareEllipse();
    std::cout << "\nPunkty elipsy Poincare:" << std::endl;
    for (size_t i = 0; i < poincareEllipse.getX().size(); ++i) {
        std::cout << std::fixed << std::setprecision(6) << poincareEllipse.getX()[i] << " " << poincareEllipse.getY()[i] << std::endl;
    }

    return 0;
}