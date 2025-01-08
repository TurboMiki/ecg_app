#include "SDsCalculator.h"
#include <vector>
#include <armadillo>

SDsCalculator::SDsCalculator() : SD1_(0.0), SD2_(0.0) {}

void SDsCalculator::calculateSDs(const std::vector<double>& intervals) {
    if (intervals.size() < 2) return;

    // Obliczanie parametrów SD1 i SD2
    
    arma::vec poincareXVec(intervals.data(), static_cast<arma::uword>(intervals.size()), false);
    arma::vec poincareYVec(poincareXVec.memptr() + 1, static_cast<arma::uword>(intervals.size() - 1), false);

    // Obliczanie ró¿nic miêdzy kolejnymi punktami na wykresie Poincaré
    arma::vec diff = poincareYVec - poincareXVec.head(poincareYVec.size());

    // Obliczanie odchylenia standardowego SDSD 
    double sdsd = arma::stddev(diff);

    // Obliczanie odchylenia standardowego NN (SDNN) 
    double sdnn = arma::stddev(poincareXVec);

    SD1_ = std::sqrt(0.5) * sdsd;
    SD2_ = std::sqrt(2 * std::pow(sdnn, 2) - 0.5 * std::pow(sdsd, 2));
}

double SDsCalculator::getSD1() const {
    return SD1_;
}

double SDsCalculator::getSD2() const {
    return SD2_;
}