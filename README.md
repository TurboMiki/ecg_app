
# **ECG App**
> Main project for DADM (kc Fabian)  

---

## **Witam wszystkich! 👋**  
W tym pliku znajdziecie wszystkie potrzebne informacje oraz instrukcje.  
W razie <span style="color: #82e332"><strong>problemów</strong></span> bądź jakichkolwiek <span style="color: #eec634"><strong>pytań/nieścisłości</strong></span> prosze pisać do mnie lub Witka na priv.

---

## **Instalacja środowiska**
Aby pracować w C/C++, potrzebny jest kompilator. Oto kilka sposobów, jak go zainstalować:  

### **Windows**  
1. **MinGW**  
   Najpopularniejszy wybór.  
   ➡️ [Tutorial z VS Code](https://code.visualstudio.com/docs/cpp/config-mingw)  
2. **Visual Studio**  
   Jeśli masz zainstalowane Visual Studio, kompilator jest już dostępny.  
3. **CLion (JetBrains)**  
   IDE od JetBrains – prawdopodobnie dostępna licencja uczelniana (warto sprawdzić).  
4. **WSL (Windows Subsystem for Linux)**  
   Polecane rozwiązanie – instalacja Ubuntu w WSL.  
   ➡️ [Tutorial z VS Code](https://code.visualstudio.com/docs/cpp/config-wsl)  

---

### **Linux**  
1. Zainstaluj kompilator za pomocą menedżera pakietów (np. `apt`, `yum`, `dnf`):  
    ```bash
    sudo apt update  
    sudo apt install build-essential  
    ```  
2. Zweryfikuj instalację:  
    ```bash
    g++ --version  
    ```  
    Jeśli komenda zwraca błąd lub nic, coś poszło nie tak.  

---

### **CMake**  
CMake jest wymagany do generowania systemów kompilacji:  
- **Windows**: Pobierz z oficjalnej strony ➡️ [CMake Download](https://cmake.org/download/)  
- **Szybki tutorial**: [Obejrzyj tutaj](https://www.youtube.com/watch?v=7YcbaupsY8I)  

Zweryfikuj instalację:  
```bash
cmake --version  
```  

Nastepnie, jeśli będziesz korzystać ze stworzonego przez mnie pliku `CMakeLists.txt` musisz ustawić zmienną środowiskową *`QT_PATH`* która będzie wskazywać na miejsce instalacji biblioteki *QT*.
W moim przypadku (WSL) jest to `/opt/Qt/6.8.0/gcc_64/lib/cmake/Qt6`. 

**Gratulacje! 🎉 Środowisko gotowe!**  

---

## **Struktura plików**
Struktura projektu wygląda następująco:  

### **Folder główny: `ecg_app/`**
#### **`build/`**  
- Tego filderu możecie nie mieć na swoim branchu, ponieważ każdy musi go sobie samemu zrobić (proszę też sprawdzić czy znajduje się on w `.gitignore`) 
- Zawiera pliki generowane przez CMake.  
- Znajduje się tu również plik wykonywalny (exec) generowany w wyniku kompilacji.  
- Nazwa pliku zależy od ustawień w `CMakeLists.txt`, w segmencie:  
    ```cmake
    project(ECGProcessing)
    ```  
- Aby uruchomić program:  
    ```bash
    ./ECGProcessing  
    ```  

#### **`include/`**  
- Przechowuje deklaracje (`*.h`).  
- Znajdziesz tu definicje klas, prototypy funkcji, stałe oraz `#include`.  
- Informuje kompilator o strukturze kodu bez definiowania szczegółów działania.

#### **`src/`**  
- Przechowuje definicje (`*.cpp`).  
- Zawiera implementacje metod i funkcji zadeklarowanych w plikach `.h`.  
- Aby zaimportować deklaracje, użyj:  
    ```cpp
    #include "MyClass.h"  
    ```  

#### **`main.cpp`**  
- Główny plik aplikacji – punkt wejściowy programu wykonywany w systemie operacyjnym.  

#### **`CMakeLists.txt`**  
- Generator systemu kompilacji dla C++.  
- Automatycznie generuje pliki i konfiguracje dla kompilatorów.  
- Zawiera:  
    - Pliki źródłowe  
    - Ustawienia kompilatora  
    - Biblioteki używane w aplikacji  
    - Ścieżkę do miejsca instalacji biblioteki QT
---

## **Struktura klas**

<span style="color: #fc187c;"><strong>❗Proszę obowiązkowo zapoznać się z tym diagramem:❗</strong></span>
- Co ma znajdować się w klasie
- Jakie typy zmiennych ma zawierać/przyjmować/zwracać klasa

Poniżej znajduje się diagram przedstawiający strukturę klas:  

![Diagram klas](<Screenshot 2024-11-19 224122.png>)  

---

## **Testy**
- Testy należy tworzyć w folderze `tests/`
- Aby wykonac test należy:
    - Stworzyć test i dodać go do `CMakeLists.txt` podobnie jak jest to zrobione dla `test_signal.cpp`
    - Zbudować aplikacje [Budowanie aplikacji](#Budowanie-Aplikacji)
    - Odpalić test

---

## **Budowanie Aplikacji**
Niestety w C/C++ aby program działał należy go najpierw skompilować. Robi się to całkiem skomplikowane kiedy mamy do czynienia z armią plików. Tutaj z pomoca przychodzi nam Cmake. Aby zbudować aplikację należy:
- Wejść do folderu build:
    ```bash
    cd build
    ```
- Następnie zbudować aplikację:
    ```bash
    cmake ..
    ```
- Potem zbudować testy:
    ```bash
    cmake --build . --target nazwa_twojego_testu
    ```
- I na końcu przetestować moduł:
    ```bash
    ctest
    ```

---

## **Git oraz GitHub**
Moi drodzy, kilka zasad odnośnie Githuba:
- ⚠️ Pierwsza rzecz którą **obowiązkowo** każdy ma zrobić to sprawdzić czy jesteście na swoim `branchu`. Jesli nie:
    ```bash
    git checkout branch-name
    ```
- ⚠️ Następną **obowiązkową** rzeczą jest fetchowanie zmian z `maina`:
    ```bash
    git merge main
    ```
- ❌ **Pod żadnym pozorem** proszę nie robić commitów na `main`.
- ❌ Proszę też nie robić merga na `maina` ze swojego brancha - tym zajmiemy się razem z Witkiem.

---

## **Klasa Signal**
Jak przyjrzycie się deklaracji/definicji klasy Signal to zobaczycie że posiada ona metodę `getTestVectors()` która zwraca sinusa. Jeśli do czegoś wam się to przyda to śmiało możecie ją wykorzystywać.
Oczywiście zachęcam do tworzenia swoich własnych danych testowych - przydadzą Wam się.

---

### **Dziękujemy za pracę nad ECG App! 🎉**

---