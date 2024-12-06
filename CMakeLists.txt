cmake_minimum_required(VERSION 3.10)

project(ECGProcessing LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ustawienie ścieżki do Qt
set(Qt6_DIR "/opt/Qt/6.8.0/gcc_64/lib/cmake/Qt6")

# Zdefiniowanie źródeł
file(GLOB_RECURSE SOURCES src/**/*.cpp)
message(STATUS "Source files found: ${SOURCES}")

# Biblioteka statyczna
add_library(ecg_app STATIC ${SOURCES})
target_include_directories(ecg_app PUBLIC 
    include 
    include/Baseline 
    include/DataReader 
    include/HRV_1
    include/HRV_2 
    include/HRV_DFA 
    include/HeartClass 
    include/RPeaks 
    include/Signal 
    include/Waves
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