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
    double conv_factor;
    double sample_rate;

    streampos bytes;
    int16_t value;
    int bitpose, bytepos, bit_in_byte;
    int bitidx, byteidx;

    vector<double> dataMLII;
    vector<double> dataV;
    vector<double> time;
    vector<char> buffer;

    void insert_data_to_subsets();
    void load_time();
    int16_t change_if_negative(int16_t input);

    public:
    DataReader(string file_path, double conv_factor = 20.0, double sample_rate = 360.0);
    ~DataReader();

    vector<double> read_MLII();
    vector<double> read_V();
    vector<double> read_time();

    void write_MLII(int samples = -1);
    void write_V(int samples = -1);
    void write_time(int samples = -1);
};

inline vector<double> DataReader::read_MLII() {
    return dataMLII;
}
inline vector<double> DataReader::read_V() {
    return dataV;
}
inline vector<double> DataReader::read_time() {
    return time;
}