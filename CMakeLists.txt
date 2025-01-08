cmake_minimum_required(VERSION 3.10)

project(ECGProcessing LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ustawienie ścieżek do Qt
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.0/msvc2022_64/lib/cmake")
set(Qt6_DIR "C:/Qt/6.9.0/msvc2022_64/lib/cmake/Qt6")
set(Qt6GuiTools_DIR "C:/Qt/6.9.0/msvc2022_64/lib/cmake/Qt6GuiTools")
set(Qt6Widgets_DIR "C:/Qt/6.9.0/msvc2022_64/lib/cmake/Qt6Widgets")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Zdefiniowanie źródeł
file(GLOB_RECURSE SOURCES src/**/*.cpp)
message(STATUS "Source files found: ${SOURCES}")

# Biblioteka statyczna
add_library(ecg_app STATIC ${SOURCES})

# Explicitly include necessary directories
target_include_directories(ecg_app PUBLIC 
    include
    include/Model
    include/Model/Baseline
    include/Model/HRV_1
    include/Model/HRV_2
    include/Model/HRV_DFA
    include/Model/DataReader
    include/Model/RPeaks
    include/Model/Signal
    include/Model/Waves
    include/Model/HeartClass

    include/View
    include/View/MainWindow

    include/ViewModel
    
)

# Znalezienie Qt
find_package(Qt6 COMPONENTS Widgets REQUIRED)
target_link_libraries(ecg_app Qt6::Widgets)

# Plik wykonywalny
add_executable(ECGProcessing main.cpp)
target_link_libraries(ECGProcessing ecg_app)

# Pobranie GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
)
FetchContent_MakeAvailable(googletest)

# Włączenie testów
enable_testing()

# Testy
add_executable(test_signal tests/test_signal.cpp)
target_link_libraries(test_signal PRIVATE gtest gtest_main ecg_app Qt6::Widgets)

include(GoogleTest)
gtest_discover_tests(test_signal)
