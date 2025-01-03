#pragma once
#include "Signal.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <chrono>
using namespace std;


class DataReader {
    private:
    static const int bitsize = 12;

    string file_path;
    ifstream input_file;
    double conv_factor;
    double sample_rate;
    chrono::duration<double, milli> time_measure;

    streampos bytes;
    int16_t value;
    int bitpose, bytepos, bit_in_byte;
    int bitidx, byteidx;

    Signal dataMLII;
    Signal dataV;
    vector<double> dataMLII_vec;
    vector<double> dataV_vec;
    vector<double>  time;
    vector<char> buffer;

    void insert_data_to_subsets();
    void load_time();
    int16_t change_if_negative(int16_t input);

    public:
    DataReader(string file_path, double conv_factor = 20.0, double sample_rate = 360.0);
    ~DataReader();

    Signal read_MLII();
    Signal read_V();

    void write_MLII(int samples = -1);
    void write_V(int samples = -1);
    void write_time(int samples = -1);
    void write_measured_time();
};

inline Signal DataReader::read_MLII() {
    return dataMLII;
}
inline Signal DataReader::read_V() {
    return dataV;
}