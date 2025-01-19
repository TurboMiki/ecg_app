#ifndef HEARTCLASS_H
#define HEARTCLASS_H

#pragma once
#include "HRV_1.h"
#include "Waves.h"
#include <vector>

// Różne typy aktywacji
enum typeOfActivation {
    DIFF_DISEASE,
    VENTRICULAR,
    SUPRAVENTRICULAR,
    ARTIFACT
};

struct activations_t {
    typeOfActivation actType; // Typ aktywacji
    int Ridx; // Indeks R-peaka
};

class HeartClass {
public:
    // Metoda process do przetwarzania danych
    void process(const std::vector<int>& rPeaks,
                 const std::vector<int>& P,
                 const std::vector<int>& QRSend,
                 const std::vector<int>& QRSonset,
                 int fs);

    // Getter do uzyskania wyników klasyfikacji
    const std::vector<activations_t>& getActivations() const;

private:
    typeOfActivation CheckAVDissociation(const std::vector<int>& P, int currR, const std::vector<int>& QRSonset);
    std::vector<activations_t> activations;
};

#endif