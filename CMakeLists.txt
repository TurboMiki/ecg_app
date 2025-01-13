cmake_minimum_required(VERSION 3.10)

project(ECGProcessing LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set Qt path
set(Qt6_DIR "/opt/Qt/6.8.0/gcc_64/lib/cmake/Qt6")
set(CMAKE_PREFIX_PATH "/opt/Qt/6.8.0/gcc_64")

# Qt settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt packages
find_package(Qt6 REQUIRED COMPONENTS 
    Core
    Gui
    Widgets 
    PrintSupport
)

# Define sources
file(GLOB_RECURSE SOURCES src/**/*.cpp)
message(STATUS "Source files found: ${SOURCES}")

# First add the Wykresy subdirectory
add_subdirectory(Wykresy)

# Static library
add_library(ecg_app STATIC ${SOURCES})
# Add resource files to library
set(RESOURCE_FILES src/View/MainWindow/resources.qrc)
qt6_add_resources(RESOURCES ${RESOURCE_FILES})
target_sources(ecg_app PRIVATE ${RESOURCES})

# Include directories										
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
    "${CMAKE_SOURCE_DIR}/Wykresy/Header Files"
)

# Link Qt libraries and visualization
target_link_libraries(ecg_app PUBLIC 
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
    visualization
)

# Executable
add_executable(ECGProcessing main.cpp)
target_link_libraries(ECGProcessing ecg_app)

# GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Tests
add_executable(test_signal tests/test_signal.cpp)
target_include_directories(test_signal PRIVATE 
    include
    include/Model
    include/View
    include/ViewModel
    "${CMAKE_SOURCE_DIR}/Wykresy/Header Files"
)											   
target_link_libraries(test_signal PRIVATE 
    gtest 
    gtest_main 
    ecg_app 
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
)