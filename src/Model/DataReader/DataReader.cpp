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
            dataMLII_vec.push_back(value / conv_factor);
        }
        else {
            dataV_vec.push_back(value / conv_factor);
        }
    }
}

void DataReader::load_time() {
    int len_data = dataMLII_vec.size();
    for (double tick = 0; tick < (1/sample_rate) * (len_data - 1); tick = tick + 1/sample_rate) {
        time.push_back(tick);
    }
}

DataReader::DataReader(string file_path, double conv_factor, double sample_rate)
: file_path(file_path), conv_factor(conv_factor), sample_rate(sample_rate) {
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
    load_time();

    dataMLII.setX(time);
    dataMLII.setY(dataMLII_vec);
    dataMLII.setSamplingRate(sample_rate);

    dataV.setX(time);
    dataV.setY(dataV_vec);
    dataV.setSamplingRate(sample_rate);
}

DataReader::~DataReader() {
    input_file.close();
}

void DataReader::write_MLII(int samples) {
    cout << "Canal MLII: " << endl;
    cout << "Data size: " << dataMLII_vec.size() << endl;
    if(samples == -1) {
        cout << "Here are all samples: " << endl;
        for(int i = 0; i < dataMLII_vec.size(); i++) {
            cout << dataMLII_vec[i] << " ";
        }
        cout << endl;
    }
    cout << "Here are first " << samples << " samples: " << endl;
    for(int i = 0; i < samples; i++) {
        cout << dataMLII_vec[i] << " ";
    }
    cout << endl;
}

void DataReader::write_V(int samples) {
    cout << "Canal V: " << endl;
    cout << "Data size: " << dataV_vec.size() << endl;
    if(samples == -1) {
        cout << "Here are all samples: " << endl;
        for(int i = 0; i < dataV_vec.size(); i++) {
            cout << dataV_vec[i] << " ";
        }
        cout << endl;
    }
    cout << "Here are first " << samples << " samples: " << endl;
    for(int i = 0; i < samples; i++) {
        cout << dataV_vec[i] << " ";
    }
    cout << endl;
}

void DataReader::write_time(int samples) {
    cout << "Time ticks: " << endl;
    cout << "Time size: " << time.size() << endl;
    if(samples == -1) {
        cout << "Here are all samples: " << endl;
        for(int i = 0; i < time.size(); i++) {
            cout << time[i] << " ";
        }
        cout << endl;
    }
    cout << "Here are the first " << samples << " samples: " << endl;
    for(int i = 0; i < samples; i++) {
        cout << time[i] << " ";
    }
    cout << endl;
}