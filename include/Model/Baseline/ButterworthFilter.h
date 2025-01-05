#pragma once
#include "Filter.h"

class ButterworthFilter : public Filter {
  private:
    int order;
    std::vector<double> freq{f1, f2};
  public:
    void set(int order, float f1, float f2);
    
    Signal applyFilter(const Signal& inputSignal) const override;
};
