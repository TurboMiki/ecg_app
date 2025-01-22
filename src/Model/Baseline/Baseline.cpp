#include "Baseline.h"
#include<vector>
#include <iostream>

using namespace std;

Baseline::Baseline() : filter(nullptr) {}

Baseline::~Baseline() {}

void Baseline::setFilter(std::unique_ptr<Filter> newFilter) {
    if (!newFilter) {
        throw std::invalid_argument("Filter cannot be null");
    }
    filter = std::move(newFilter);
}

Signal Baseline::filterSignal(const Signal& inputSignal) {
    if (!filter) {
        throw std::runtime_error("No filter set");
    }
    outSignal = filter->applyFilter(inputSignal);
    return outSignal;
}

Signal Baseline::getSignal() const {
    return outSignal;
}