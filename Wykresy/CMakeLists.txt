cmake_minimum_required(VERSION 3.16)
project(visualization VERSION 0.1 LANGUAGES CXX)

# Qt settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Qt path
set(CMAKE_PREFIX_PATH "/opt/Qt/6.8.0/gcc_64")

# Find Qt packages
find_package(Qt6 REQUIRED COMPONENTS 
    Core
    Gui
    Widgets 
    PrintSupport
)

# Specify header files explicitly
set(VISUALIZATION_HEADERS
    "Header Files/qcustomplot.h"
    "Header Files/basic_plot.h"
    "Header Files/table.h"
    "Header Files/histogram_plot.h"
    "Header Files/waves_plot.h"
)

set(VISUALIZATION_SOURCES
    "Source Files/qcustomplot.cpp"
    "Source Files/basic_plot.cpp"
    "Source Files/table.cpp"
    "Source Files/histogram_plot.cpp"
    "Source Files/waves_plot.cpp"
)

# Create the library
add_library(visualization STATIC 
    ${VISUALIZATION_HEADERS}
    ${VISUALIZATION_SOURCES}
)

# Include directories
target_include_directories(visualization PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/Header Files"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/Model"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/Model/Signal"
)

# Link against Qt
target_link_libraries(visualization PUBLIC 
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets 
    Qt6::PrintSupport
    ecg_app
)