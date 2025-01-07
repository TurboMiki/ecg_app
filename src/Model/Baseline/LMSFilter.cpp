#include "LMSFilter.h"
#include <LMS/AdaptiveFilter.c>

static AfData Adata = {
		STEPSIZE,
        REGULARIZATION,
		NUM_TAPS,
		inBuffer,
        0, /* initial buffer index */
		weights,
		0.0 /* initial error */
};

Signal LMSFilter::applyFilter(const Signal& inputSignal, const Signal& refSignal) const {
    //refSignal -> pewnie jakiś losowy z tej bazy(lub przefiltrowany w matlabie i zapisany z powrotem do tego .dat(?))
    std::vector<double> oldY = inputSignal.getY;
    std::vector<double> refY = refSignal.getY;
    std::vector<double> newY;
    
    
    for(i=oldY.begin(); i != oldY.end(); i++) {
        newY.push_back(AdaptiveFilterRun(oldY[i], refY[i], &Adata));
    }

    inputSignal::SetY(newY);
}
