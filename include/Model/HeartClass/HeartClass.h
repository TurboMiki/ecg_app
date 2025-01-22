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

class HeartClass {
public:
    // Metoda process do przetwarzania danych
    void process(const std::vector<int>& rPeaks,
                 const std::vector<int>& P,
                 const std::vector<int>& QRSend,
                 const std::vector<int>& QRSonset,
                 int fs);

    // Gettery do uzyskania wystapien poszczegolnych aktywacji
    int getSupraventricularCount() const;
    int getVentricularCount() const;
    int getDiffDiseaseCount() const;
    int getArtifactCount() const;
    int getTotalCount() const;

private:
    int supraventricularCount = 0;
    int ventricularCount = 0;
    int diffDiseaseCount = 0;
    int artifactCount = 0;
    int totalCount = 0;

    typeOfActivation CheckAVDissociation(const std::vector<int>& P, int currR, const std::vector<int>& QRSonset);
};

#endif