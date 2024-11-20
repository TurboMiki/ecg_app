
# **ECG App**
> Main project for DADM (kc Fabian)  

---

## **Witam wszystkich! 👋**  
W tym pliku znajdziecie wszystkie potrzebne informacje oraz instrukcje.  

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

**Gratulacje! 🎉 Środowisko gotowe!**  

---

## **Struktura plików**
Struktura projektu wygląda następująco:  

### **Folder główny: `ecg_app/`**
#### **`build/`**  
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

---

## **Struktura klas**
Poniżej znajduje się diagram przedstawiający strukturę klas:  

![Diagram klas](<Screenshot 2024-11-19 224122.png>)  

---

### **Dziękujemy za użycie ECG App! 🎉**

---