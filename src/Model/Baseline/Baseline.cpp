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

// 1D convolution function.
vector<vector<double>>& Conv1D(vector<vector<double>> input, vector<vector<double>> kernel)
{   
    int inputHeight = input.size();
    int inputWidth = input[0].size();
    int kernelHeight = kernel.size();
    int kernelWidth = kernel[0].size();

    int outputHeight = inputHeight - kernelHeight + 1;
    int outputWidth = inputWidth - kernelWidth + 1;

    static vector<vector<double>> output;
    for (int i=0; i<outputHeight; i++)
        output.push_back(vector<double> (outputWidth, 0.0));

    double res = 0; // This holds the convolution results for an index.

	// Fill output matrix: rows -> i, cols -> j
    for (int i=0; i<outputHeight; i++){
        for (int j=0; j<outputWidth; j++){

            for (int k=0; k<kernelHeight; k++){
                for (int l=0; l<kernelWidth; l++){
                    res += input[i+k][j+l] * kernel[k][l];
                }
            }
            output[i][j] = res;
            res = 0;
        }
    }

    return output;
}

//For testing purposes later
/* int main(){
  vector<vector<double>> input({{0,1,2}, {3,4,5}, {6,7,8}});
  vector<vector<double>> kernel({{0,1}, {2,3}});
  vector<vector<double>>& output = conv1D(input, kernel);

  for (int i=0; i<output.size(); i++)
    for (int j=0; j<output[0].size(); j++)
      std::cout << output[i][j] << "\t";

  return 0;
} */