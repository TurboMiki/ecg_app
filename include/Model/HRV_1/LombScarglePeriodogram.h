#include <vector>
#include <Signal.h>

class LombScarglePeriodogram{
    public:
         LombScarglePeriodogram(const Signal& ecg);
    protected:
        std::vector<double> periodogram_;
        std::vector<double> frequencies_;
    private:
        void calculatePeriodogramLS(const Signal& ecg);
};

