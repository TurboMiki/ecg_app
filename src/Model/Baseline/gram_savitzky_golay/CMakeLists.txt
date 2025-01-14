cmake_minimum_required(VERSION 3.10)

project(gram_savitzky_golay VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find required packages
find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED)

# Create library
add_library(gram_savitzky_golay STATIC
    src/gram_savitzky_golay.cpp
    src/spatial_filters.cpp
)

# Include directories
target_include_directories(gram_savitzky_golay
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${EIGEN3_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
)

# Link dependencies
target_link_libraries(gram_savitzky_golay
    PUBLIC
        Eigen3::Eigen
        Boost::boost
)