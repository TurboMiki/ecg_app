#pragma once
#include "Signal.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
using namespace std;


class DataReader {
    private:
    static const int bitsize = 12;

    string file_path;
    ifstream input_file;
    float conv_factor;

    streampos bytes;
    int16_t value;
    int bitpose, bytepos, bit_in_byte;
    int bitidx, byteidx;

    vector<double> dataMLII;
    vector<double> dataV;
    vector<char> buffer;

    void insert_data_to_subsets();
    int16_t change_if_negative(int16_t input);

    public:
    DataReader(string file_path, float conv_factor = 20.0);
    ~DataReader();

    vector<double> read_MLII();
    vector<double> read_V();

    void write_MLII(int samples = -1);
    void write_V(int samples = -1);
};

inline vector<double> DataReader::read_MLII() {
    return dataMLII;
}
inline vector<double> DataReader::read_V() {
    return dataV;
}