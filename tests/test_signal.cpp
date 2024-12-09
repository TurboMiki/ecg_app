#define _USE_MATH_DEFINES
#include "gtest/gtest.h"
#include "Signal.h"

#include <cmath>

TEST(SignalTest, TestSineWave) {

    Signal testSignal = Signal::getTestVectors();

    ASSERT_EQ(testSignal.getSize(), 1000) << "Signal should have 1000 samples";

    ASSERT_EQ(testSignal.getSamplingRate(), 250) << "Sampling rate should be 250 Hz";

    const auto& x = testSignal.getX();
    const auto& y = testSignal.getY();

    for (size_t i = 0; i < x.size(); ++i) {
        double expectedY = std::sin(2 * M_PI * 1 * x[i]);
        EXPECT_NEAR(y[i], expectedY, 1e-5) << "Mismatch at index " << i;
    }
}
