cmake_minimum_required(VERSION 3.16)
project(visualization VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(VISUALIZATION_SOURCES
    "Source Files/qcustomplot.cpp"
    "Source Files/basic_plot.cpp"
    "Source Files/table.cpp"
    "Source Files/histogram_plot.cpp"
    "Source Files/waves_plot.cpp"
)

add_library(visualization STATIC ${VISUALIZATION_SOURCES})

# Add include directories for Signal class
target_include_directories(visualization PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/Header Files"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/Model"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/Model/Signal"
)

# Link against the ecg_app library to access Signal class
target_link_libraries(visualization 
    PUBLIC 
    Qt6::Widgets
    ecg_app
)