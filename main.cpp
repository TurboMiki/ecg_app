#include "ECG_Input.h"
using namespace  std;

// usage example:
int main() {
    // path to file for mit-database
    string path = "C:/Users/lenovo/OneDrive/Dokumenty/MATLAB/DADM/mit-bih-arrhythmia-database-1.0.0/100.dat";
    double conv = 200.0;

    DataReader ECG_data(path, conv);
    ECG_data.write_V(100);
    ECG_data.write_MLII(50);

    vector<double> V = ECG_data.read_V();
    vector<double> MLII = ECG_data.read_MLII();

}