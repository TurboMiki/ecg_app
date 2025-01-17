cmake_minimum_required(VERSION 3.10)

project(libbutterworth)

# Create library
add_library(libbutterworth STATIC
    biquad.cpp
    butterworth.cpp
    filter_design.cpp
    utils.cpp
)

# Add include directories
target_include_directories(libbutterworth
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# If we need to set specific compiler flags
target_compile_features(libbutterworth PUBLIC cxx_std_17)