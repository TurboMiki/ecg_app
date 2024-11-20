ECG App
Main project for DADM (kc Fabian)

Witam wszystkich! 👋
W tym pliku znajdziecie wszystkie potrzebne informacje oraz instrukcje.

Instalacja środowiska
Aby pracować w C/C++, potrzebny jest kompilator. Oto kilka sposobów, jak go zainstalować:

Windows
MinGW
Najpopularniejszy wybór.
➡️ Tutorial z VS Code
Visual Studio
Jeśli masz zainstalowane Visual Studio, kompilator jest już dostępny.
CLion (JetBrains)
IDE od JetBrains – prawdopodobnie dostępna licencja uczelniana (warto sprawdzić).
WSL (Windows Subsystem for Linux)
Polecane rozwiązanie – instalacja Ubuntu w WSL.
➡️ Tutorial z VS Code
Linux
Zainstaluj kompilator za pomocą menedżera pakietów (np. apt, yum, dnf):
bash
Skopiuj kod
sudo apt update  
sudo apt install build-essential  
Zweryfikuj instalację:
bash
Skopiuj kod
g++ --version  
Jeśli komenda zwraca błąd lub nic, coś poszło nie tak.
CMake
CMake jest wymagany do generowania systemów kompilacji:

Windows: Pobierz z oficjalnej strony ➡️ CMake Download
Szybki tutorial: Obejrzyj tutaj
Zweryfikuj instalację:

bash
Skopiuj kod
cmake --version  
Gratulacje! 🎉 Środowisko gotowe!

Struktura plików
Struktura projektu wygląda następująco:

Folder główny: ecg_app/
build/
Zawiera pliki generowane przez CMake.
Znajduje się tu również plik wykonywalny (exec) generowany w wyniku kompilacji.
Nazwa pliku zależy od ustawień w CMakeLists.txt, w segmencie:
cmake
Skopiuj kod
project(ECGProcessing)
Aby uruchomić program:
bash
Skopiuj kod
./ECGProcessing  
include/
Przechowuje deklaracje (*.h).
Znajdziesz tu definicje klas, prototypy funkcji, stałe oraz #include.
Informuje kompilator o strukturze kodu bez definiowania szczegółów działania.
src/
Przechowuje definicje (*.cpp).
Zawiera implementacje metod i funkcji zadeklarowanych w plikach .h.
Aby zaimportować deklaracje, użyj:
cpp
Skopiuj kod
#include "MyClass.h"  
main.cpp
Główny plik aplikacji – punkt wejściowy programu wykonywany w systemie operacyjnym.
CMakeLists.txt
Generator systemu kompilacji dla C++.
Automatycznie generuje pliki i konfiguracje dla kompilatorów.
Zawiera:
Pliki źródłowe
Ustawienia kompilatora
Biblioteki używane w aplikacji
Struktura klas
Poniżej znajduje się diagram przedstawiający strukturę klas:


Dziękujemy za użycie ECG App! 🎉