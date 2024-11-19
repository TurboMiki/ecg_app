#pragma once
#include "Signal.h"

class Filter {
private:
    int butterworthOrder; 
    float butterworthFreq;

    int savGolFilterLength;
    int savGolOrder;

    // TODO

public:
    Filter();
    ~Filter();

    void setButterworthParams(int order, float freq);
    void setSavGolParams(int filterLength, int order);

    Signal applyButterworth(const Signal& inputSignal);
    Signal applySavGol(const Signal& inputSignal);

    // TODO -> Wogóle daj znać co sadzisz o takiej strukturze tej klasy - czy moze chciałabyś cos zmienić
};
