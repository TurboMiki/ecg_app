# ecg_app
Main project for DADM (kc fabian)

Witam wszystkich! 

W tym pliku znajdziecie wszystkie potrzebne informacje oraz instrukcje.

### Instalacja środowiska:
Do zrobienia czegokolwiek w C/C++ potrzebny jest kompilator. Ten można pobrać na kilka sposobów:

> Windows:
    Najpopularniejszym jest MinGW -> https://code.visualstudio.com/docs/cpp/config-mingw <- Link do tutorialu z VS Codem
    Jesli macie zainstalowane Visual Studio to nie musicie sie przejmować - kompilator jest już z wami.
    Trzecią opcją jest zainstalowanie CLion (IDE od JetBrains -> chyba mamy licencje z uczelni ale idk)

    Polecana opcją jest ogarnięcie WSL na Windowsie (najlepiej Ubuntu) i na nim postawienie środowiska -> https://code.visualstudio.com/docs/cpp/config-wsl <- Tutaj szybki tutorial

> Linux:
    W zalezności od dystrybucji będzie się zmieniał tylko package manager (apt, yum, dnf, etc. ..). 
    Aby pobrać i zainstalowac kompilator wystarczy wykonać komendy:
        sudo apt update
        sudo apt install build-essential
    
    Aby sprawdzic czy kompilator został zainstalowany poprawnie można wykonac komendę:
        g++ --version 
    Jeśli ta komenda zwraca błąd/nie zwraca nic to znaczy że coś jest nie tak D:

Kolejnym krokiem jest zainstalowanie CMake -> https://www.youtube.com/watch?v=7YcbaupsY8I <- szybki tutorial (Dla Windows lepiej zainstalować ze stronki: https://cmake.org/download/ ). POdobnie jak w poprzednik kroku komenda 'cmake --version' weryfikuje czy instalacja przebiegła poprawnie.

    Gratulacje - środowisko postawione! Czas przejśc dalej.

### Struktura plików:
#### ecg_app/
##### build/
    Tutaj znajdują się pliki generowane przez CMake - proszę ich nie tykać. W tym folderze znajduje się też plik wykonywalny (Exec) tworzony na wskutek 
    kompilacji kodu.Plik ten bedzie sie nazywał tak jak zostanie to ustawione w pliku CMakeLists.txt w segmencie "project(ECGProcessing)".
    Aby go odpalić należy uzyc komendy ./ECGProcessing lub podobnej (zleżnie od wybranego środowiska).

##### include/
    Tutaj znajdują się deklaracje ('.h'). Pliki w tym folderze zawierają definicje klas, prototypy funkcji stałe oraz include.
    To tutaj mówisz kompilatorowi, co istnieje w twoim kodzie, bez faktycznego definiowania jego działania.
    Pliki z tego folderu są współdzielone z pozostałymi plikami, aby poinformować resztę o strukturze kodu.

##### src/
    Tutaj znajdują sie definicje ('.cpp'). Pliki w tym folderze zawierają rzeczywistą implementację metod/funkcji zadeklarowanych w plikach '.h'. 
    Aby użyć deklaracji należy dołączyć na górze plik z rozszerzeniem '.h' ,np. #include "MyClass.h" 

##### main.cpp
    Główny plik aplikacji. Jest to wyznaczony entry point do programu, który jest wykonywany w środowisku hostowanym (czyli z systemem operacyjnym).

##### CMakeLists.txt
    CMake to generator systemu kompilacji dla C++. W tym pliku definiuje sie sposób budowania projektu. Automatycznie generuje on pliki i konfiguracje dla 
    kompilatorów zamiast ręcznie wpisywac polecenia do kompilacji kodu. Zawiera informacje na temat kompilowanych plików, ustawień kompilatora, bibliotek które
    zostana wykorzystane w aplikacji. 

### Struktura klas:
Tutaj załączam screena z diagramem klas.
![alt text](<Screenshot 2024-11-19 224122.png>)

