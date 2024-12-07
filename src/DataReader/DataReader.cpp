#include "DataReader.h"

int16_t DataReader::change_if_negative(int16_t input) {
    input |= 0xF000;
    return input;
}

void DataReader::insert_data_to_subsets() {
    while (bitpose + bitsize <= bytes * 8) {

        bytepos = bitpose / 8;
        bit_in_byte = bitpose % 8;
        value = 0;

        for (int i = 0; i < bitsize; i++) {
            byteidx = bytepos + (bit_in_byte + i) / 8;
            bitidx = (bit_in_byte + i) % 8;
            if (buffer[byteidx] & (1 << bitidx)) {
                value |= (1 << i);
            }
        }
        if(value & 0x800)
            value = change_if_negative(value);
        bitpose += bitsize;
        if (bit_in_byte) {
            dataMLII.push_back(value / conv_factor);
        }
        else {
            dataV.push_back(value / conv_factor);
        }
    }
}

DataReader::DataReader(string file_path, float conv_factor)
: file_path(file_path), conv_factor(conv_factor) {
    input_file.open(file_path, ios::in | ios::binary);

    input_file.seekg(0, ios::end);
    bytes = input_file.tellg();
    input_file.seekg(0, ios::beg);

    buffer.resize(bytes);
    input_file.read(buffer.data(), bytes);

    value = 0;
    bitpose = 0;
    bytepos = 0;
    bit_in_byte = 0;
    bitidx = 0;
    byteidx = 0;

    insert_data_to_subsets();
}

DataReader::~DataReader() {
    input_file.close();
}

void DataReader::write_MLII(int samples) {
    cout << "Canal MLII: " << endl;
    cout << "Data size: " << dataMLII.size() << endl;
    if(samples == -1) {
        cout << "Here are all samples: " << endl;
        for(int i = 0; i < dataMLII.size(); i++) {
            cout << dataMLII[i] << " ";
        }
        cout << endl;
    }
    cout << "Here are first " << samples << " samples: " << endl;
    for(int i = 0; i < samples; i++) {
        cout << dataMLII[i] << " ";
    }
    cout << endl;
}

void DataReader::write_V(int samples) {
    cout << "Canal V: " << endl;
    cout << "Data size: " << dataV.size() << endl;
    if(samples == -1) {
        cout << "Here are all samples: " << endl;
        for(int i = 0; i < dataV.size(); i++) {
            cout << dataV[i] << " ";
        }
        cout << endl;
    }
    cout << "Here are first " << samples << " samples: " << endl;
    for(int i = 0; i < samples; i++) {
        cout << dataV[i] << " ";
    }
    cout << endl;
}