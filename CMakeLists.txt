cmake_minimum_required(VERSION 3.10)

project(ECGProcessing)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

file(GLOB SOURCES src/*.cpp)

add_library(ecg_app STATIC ${SOURCES})

add_executable(ECGProcessing main.cpp)
target_link_libraries(ECGProcessing ecg_app)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
)
FetchContent_MakeAvailable(googletest)

enable_testing()

################## TESTS #####################

add_executable(test_signal tests/test_signal.cpp)

target_link_libraries(test_signal PRIVATE gtest gtest_main ecg_app)

include(GoogleTest)
gtest_discover_tests(test_signal)
