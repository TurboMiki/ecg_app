cmake_minimum_required(VERSION 3.16)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not allowed.")
endif()

project(
  baseline_filters
  LANGUAGES CXX C
  VERSION 0.1)

# Find required packages for Savitzky-Golay filter
find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED)

# Setup googletest
set(INSTALL_GTEST OFF CACHE BOOL "Install Googletest")
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add LMS library
add_library(lms_filter STATIC
    LMS/src/AdaptiveFilter.c
)
target_include_directories(lms_filter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/LMS/src
)

# Add Savitzky-Golay library (using the simplified CMakeLists.txt)
add_subdirectory(gram_savitzky_golay)

# Configure libbutterworth
add_library(
  libbutterworth STATIC
  libbutterworth/biquad.h
  libbutterworth/biquad.cpp
  libbutterworth/butterworth.h
  libbutterworth/butterworth.cpp
  libbutterworth/filter_design.h
  libbutterworth/filter_design.cpp
  libbutterworth/utils.h
  libbutterworth/utils.cpp)

# Include directories for libbutterworth
target_include_directories(
  libbutterworth PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/libbutterworth
    ${CMAKE_SOURCE_DIR}/include/Model/Baseline
)

# Now set up the baseline library
add_library(baseline STATIC
    Baseline.cpp
    ButterworthFilter.cpp
    MovingMeanFilter.cpp
    SavitzkyGolayFilter.cpp
    LMSFilter.cpp
)

# Add include directories for baseline
target_include_directories(baseline
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/Model
        ${CMAKE_SOURCE_DIR}/include/Model/Baseline
        ${CMAKE_SOURCE_DIR}/include/Model/Signal
        ${CMAKE_CURRENT_SOURCE_DIR}/gram_savitzky_golay/include
        ${CMAKE_CURRENT_SOURCE_DIR}/LMS/src
        ${EIGEN3_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
)

# Link baseline with all filter libraries
target_link_libraries(baseline
    PUBLIC
        libbutterworth
        lms_filter
        gram_savitzky_golay
        Eigen3::Eigen
        Boost::boost
)

# Tests setup
enable_testing()
add_executable(
  filter_tests
  libbutterworth/butterworth_tests.cpp 
  libbutterworth/biquad_tests.cpp
  libbutterworth/filter_design_tests.cpp 
  libbutterworth/utils_tests.cpp)
target_link_libraries(filter_tests gtest_main libbutterworth)
include(GoogleTest)
gtest_discover_tests(filter_tests)