#pragma once
#include <Signal.h>
#include <fstream>
#include <vector>
#include <cstdint>
#include <chrono>
using namespace std;

/*
 * Klasa służy do odczytu plików binarnych (*.dat)
 * z bazy MIT-BIH Arrhytmia zawierających sygnały EKG w dwóch kanałach MLII i V.
 */
class DataReader {
private:
    static const int bitsize = 12;

    string file_path;
    ifstream input_file;
    double conv_factor = 200;
    double sample_rate = 360;
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
    /*
    DataReader(string file_path, double conv_factor = 200.0, double sample_rate = 360.0)
        Konstrukcja obiektu DataReader
            1.	Wejście metody:
                •	file_path – ścieżka do pliku
                •	conv_factor – współczynnik konwersji (adu do mV)
                •	sample_rate – częstotliwość próbkowania
     */
    DataReader(string file_path, double conv_factor = 200.0, double sample_rate = 360.0);
    /*
    DataReader()
        Konstrukcja obiektu DataReader (argumenty conv_factor i sample_rate ustawiane są na wartości domyślne), ścieżkę należy ustawić później
     */
    DataReader();
    /* ~DataReader()
        Zwolnienie obiektu DataReader
    */
    ~DataReader();

    /*
    inline Signal read_MLII()
        Odczytanie próbek z kanału MLII
            1.	Wyjście metody: obiekt Signal, w którym zapisano próbki czasu oraz próbki sygnału EKG z kanału MLII

     */
    Signal read_MLII();
     /*
    inline Signal read_V()
        Odczytanie próbek z kanału V
            1.	Wyjście metody: obiekt Signal, w którym zapisano próbki czasu oraz próbki sygnału EKG z kanału V

     */
    Signal read_V();

     /*
    void set_path(std::string file_path)
        Ustawienie ścieżki do pliku binarnego
            1.	Wejście metody:
                •	file_path – ścieżka do pliku
     */
    void set_path(string filePath);

    /*
    void read_file()
        Wczytanie danych, w przypadku gdy ścieżka jest ustalona – w przeciwnym wypadku zwraca wyjątek.
        Dane są ładowane do dwóch kontenerów, które odpowiadają dwóm wczytanym kanałom.
        Poza próbkami sygnału EKG ustawiany jest czas oraz częstotliwość próbkowania.
        Jeśli obiekt zawiera już jakieś próbki, zostają one zastąpione nowymi.
     */
    void read_file();
    /*
    void write_MLII(int samples = -1)
        Wypisanie próbek EKG z kanału MLII
            1.	Wejście metody:
                •	samples – liczba próbek do wypisania, domyślnie wszystkie
     */
    void write_MLII(int samples = -1);
    /*
    void write_V(int samples = -1)
        Wypisanie próbek EKG z kanału V
            1.	Wejście metody:
                •	samples – liczba próbek do wypisania, domyślnie wszystkie
     */
    void write_V(int samples = -1);
    /*
    void write_time(int samples = -1)
        Wypisanie próbek czasu
            1.	Wejście metody:
                •	samples – liczba próbek do wypisania, domyślnie wszystkie
     */
    void write_time(int samples = -1);
    /*
    void write_measured_time()
        Wypisanie czasu wczytania pliku (w ms)
     */
    void write_measured_time();
};

inline Signal DataReader::read_MLII() {
    return dataMLII;
}
inline Signal DataReader::read_V() {
    return dataV;
}
