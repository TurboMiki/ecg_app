#include "DataReader.h"
#include <iostream>

int16_t DataReader::change_if_negative(int16_t input) {
    input |= 0xF000;
    return input;
}

void DataReader::insert_data_to_subsets() {
    if(!dataMLII_vec.empty()) {
        dataMLII_vec.clear();
    }
    if(!dataV_vec.empty()) {
        dataV_vec.clear();
    }

    bitpose = 0; // Reset bit position
    while (bitpose + bitsize <= bytes * 8) {
        bytepos = bitpose / 8;
        bit_in_byte = bitpose % 8;
        value = 0;

        for (int i = 0; i < bitsize; i++) {
            byteidx = bytepos + (bit_in_byte + i) / 8;
            bitidx = (bit_in_byte + i) % 8;
            if (byteidx < buffer.size() && buffer[byteidx] & (1 << bitidx)) {
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
    if(!time.empty()) {
        time.clear();
    }

    int len_data = dataMLII_vec.size();
    time.reserve(len_data);
    for (int i = 0; i < len_data; ++i) {
        time.push_back(i / sample_rate);
    }
}

void DataReader::set_path(string filePath) {
    this->file_path = filePath;
    if(input_file.is_open()) {
        input_file.close();
    }
}

void DataReader::read_file() {
    try {
        if(input_file.is_open()) {
            input_file.close();
        }

        input_file.open(file_path, ios::in | ios::binary);
        if (!input_file.is_open()) {
            throw std::runtime_error("Could not open file: " + file_path);
        }

        input_file.seekg(0, ios::end);
        bytes = input_file.tellg();
        input_file.seekg(0, ios::beg);

        buffer.resize(bytes);
        input_file.read(buffer.data(), bytes);

        if (input_file.fail()) {
            throw std::runtime_error("Failed to read file data");
        }

        auto start = std::chrono::high_resolution_clock::now();
        insert_data_to_subsets();
        auto stop = std::chrono::high_resolution_clock::now();
        time_measure = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        
        load_time();

        dataMLII.setX(time);
        dataMLII.setY(dataMLII_vec);
        dataMLII.setSamplingRate(sample_rate);

        dataV.setX(time);
        dataV.setY(dataV_vec);
        dataV.setSamplingRate(sample_rate);

    } catch (const std::exception& e) {
        std::cout << "Error in DataReader::readFile: " << e.what() << std::endl;
        if(input_file.is_open()) {
            input_file.close();
        }
        throw;
    }
}

DataReader::DataReader(string file_path, double conv_factor, double sample_rate) 
    : file_path(file_path), conv_factor(conv_factor), sample_rate(sample_rate) {
    try {
        input_file.open(file_path, ios::in | ios::binary);
        if (!input_file.is_open()) {
            throw std::runtime_error("Could not open file: " + file_path);
        }

        input_file.seekg(0, ios::end);
        bytes = input_file.tellg();
        input_file.seekg(0, ios::beg);

        buffer.resize(bytes);
        input_file.read(buffer.data(), bytes);

        if (input_file.fail()) {
            throw std::runtime_error("Failed to read file data");
        }

        value = 0;
        bitpose = 0;
        bytepos = 0;
        bit_in_byte = 0;
        bitidx = 0;
        byteidx = 0;

        // Preserved timing measurement
        auto start = std::chrono::high_resolution_clock::now();
        insert_data_to_subsets();
        auto stop = std::chrono::high_resolution_clock::now();
        time_measure = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        
        load_time();

        dataMLII.setX(time);
        dataMLII.setY(dataMLII_vec);
        dataMLII.setSamplingRate(sample_rate);

        dataV.setX(time);
        dataV.setY(dataV_vec);
        dataV.setSamplingRate(sample_rate);

    } catch (const std::exception& e) {
        std::cout << "Error in DataReader constructor: " << e.what() << std::endl;
        if(input_file.is_open()) {
            input_file.close();
        }
        throw;
    }
}

DataReader::DataReader() : conv_factor(200.0), sample_rate(360.0) {
    value = 0;
    bitpose = 0;
    bytepos = 0;
    bit_in_byte = 0;
    bitidx = 0;
    byteidx = 0;
}

DataReader::~DataReader() {
    if(input_file.is_open()) {
        input_file.close();
    }
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

void DataReader::write_measured_time() {
    cout << "Time measurement: " << time_measure.count() << endl;
}

// #include "DataReader.h"
// #include <iostream>

// int16_t DataReader::change_if_negative(int16_t input) {
//     input |= 0xF000;
//     return input;
// }

// void DataReader::insert_data_to_subsets() {
//     if(!dataMLII_vec.empty()) {
//         dataMLII_vec.clear();
//     }
//     if(!dataV_vec.empty()) {
//         dataV_vec.clear();
//     }

//     bitpose = 0; // Reset bit position
//     while (bitpose + bitsize <= bytes * 8) {
//         bytepos = bitpose / 8;
//         bit_in_byte = bitpose % 8;
//         value = 0;

//         for (int i = 0; i < bitsize; i++) {
//             byteidx = bytepos + (bit_in_byte + i) / 8;
//             bitidx = (bit_in_byte + i) % 8;
//             if (byteidx < buffer.size() && buffer[byteidx] & (1 << bitidx)) {
//                 value |= (1 << i);
//             }
//         }
//         if(value & 0x800)
//             value = change_if_negative(value);
//         bitpose += bitsize;
//         if (bit_in_byte) {
//             dataMLII_vec.push_back(value / conv_factor);
//         }
//         else {
//             dataV_vec.push_back(value / conv_factor);
//         }
//     }
// }

// void DataReader::load_time() {
//     if(!time.empty()) {
//         time.clear();
//     }

//     int len_data = dataMLII_vec.size();
//     time.reserve(len_data); // Pre-allocate memory
//     for (int i = 0; i < len_data; ++i) {
//         time.push_back(i / sample_rate);
//     }
// }

// void DataReader::setPath(string filePath) {
//     this->file_path = filePath;
//     // Close the file if it's already open
//     if(input_file.is_open()) {
//         input_file.close();
//     }
// }

// void DataReader::readFile() {
//     try {
//         // Close the file if it's already open
//         if(input_file.is_open()) {
//             input_file.close();
//         }

//         input_file.open(file_path, ios::in | ios::binary);
//         if (!input_file.is_open()) {
//             throw std::runtime_error("Could not open file: " + file_path);
//         }

//         // Get file size
//         input_file.seekg(0, ios::end);
//         bytes = input_file.tellg();
//         input_file.seekg(0, ios::beg);

//         // Read file into buffer
//         buffer.resize(bytes);
//         input_file.read(buffer.data(), bytes);

//         if (input_file.fail()) {
//             throw std::runtime_error("Failed to read file data");
//         }

//         // Process the data
//         insert_data_to_subsets();
//         load_time(); // Add this line to reload time vector

//         // Create Signal objects
//         dataMLII.setX(time);
//         dataMLII.setY(dataMLII_vec);
//         dataMLII.setSamplingRate(sample_rate);

//         dataV.setX(time);
//         dataV.setY(dataV_vec);
//         dataV.setSamplingRate(sample_rate);

//     } catch (const std::exception& e) {
//         std::cout << "Error in DataReader::readFile: " << e.what() << std::endl;
//         // Clean up in case of error
//         if(input_file.is_open()) {
//             input_file.close();
//         }
//         throw; // Re-throw the exception for the caller to handle
//     }
// }

// void DataReader::write_MLII(int samples) {
//     cout << "Canal MLII: " << endl;
//     cout << "Data size: " << dataMLII_vec.size() << endl;
//     if(samples == -1) {
//         cout << "Here are all samples: " << endl;
//         for(int i = 0; i < dataMLII_vec.size(); i++) {
//             cout << dataMLII_vec[i] << " ";
//         }
//         cout << endl;
//     }
//     cout << "Here are first " << samples << " samples: " << endl;
//     for(int i = 0; i < samples; i++) {
//         cout << dataMLII_vec[i] << " ";
//     }
//     cout << endl;
// }

// void DataReader::write_V(int samples) {
//     cout << "Canal V: " << endl;
//     cout << "Data size: " << dataV_vec.size() << endl;
//     if(samples == -1) {
//         cout << "Here are all samples: " << endl;
//         for(int i = 0; i < dataV_vec.size(); i++) {
//             cout << dataV_vec[i] << " ";
//         }
//         cout << endl;
//     }
//     cout << "Here are first " << samples << " samples: " << endl;
//     for(int i = 0; i < samples; i++) {
//         cout << dataV_vec[i] << " ";
//     }
//     cout << endl;
// }

// void DataReader::write_time(int samples) {
//     cout << "Time ticks: " << endl;
//     cout << "Time size: " << time.size() << endl;
//     if(samples == -1) {
//         cout << "Here are all samples: " << endl;
//         for(int i = 0; i < time.size(); i++) {
//             cout << time[i] << " ";
//         }
//         cout << endl;
//     }
//     cout << "Here are the first " << samples << " samples: " << endl;
//     for(int i = 0; i < samples; i++) {
//         cout << time[i] << " ";
//     }
//     cout << endl;
// }

// void DataReader::write_measured_time() {
//     cout << "Time measurement: " << time_measure.count() << endl;
// }

// DataReader::DataReader() {

// }

// DataReader::~DataReader() {}
