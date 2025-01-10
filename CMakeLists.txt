cmake_minimum_required(VERSION 3.10)

project(ECGProcessing LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ustawienie ścieżki do Qt
set(Qt6_DIR $ENV{QTDIR})
find_package(Qt6 COMPONENTS Widgets REQUIRED HINTS $ENV{QTDIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Zdefiniowanie źródeł
file(GLOB_RECURSE SOURCES src/**/*.cpp)
message(STATUS "Source files found: ${SOURCES}")

# First add the Wykresy subdirectory so its directories are available
add_subdirectory(Wykresy)

# Biblioteka statyczna
add_library(ecg_app STATIC ${SOURCES})
# Dodanie plików zasobów do biblioteki
set(RESOURCE_FILES src/View/MainWindow/resources.qrc)
qt6_add_resources(RESOURCES ${RESOURCE_FILES})
target_sources(ecg_app PRIVATE ${RESOURCES})

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
    "${CMAKE_SOURCE_DIR}/Wykresy/Header Files"  # Add this line to access visualization headers
)

# Link Qt libraries and visualization
target_link_libraries(ecg_app PUBLIC 
    Qt6::Widgets
    visualization
)

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
target_include_directories(test_signal PRIVATE 
    include
    include/Model
    include/View
    include/ViewModel
    "${CMAKE_SOURCE_DIR}/Wykresy/Header Files"  # Add this line too if tests need visualization headers
)											   
target_link_libraries(test_signal PRIVATE 
    gtest 
    gtest_main 
    ecg_app 
    Qt6::Widgets
)

include(GoogleTest)
gtest_discover_tests(test_signal)