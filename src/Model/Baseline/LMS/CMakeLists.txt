cmake_minimum_required(VERSION 3.10)
project(AdaptiveFilter C)

# Create the adaptive filter library
add_library(adaptive_filter STATIC
    src/AdaptiveFilter.c
)

# Add include directories
target_include_directories(adaptive_filter
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)