#include <gtest/gtest.h>
#include "HysteresisThreshold.hpp"



/* Test if the activation works */
TEST(HysteresisThresholdTest, ActivationTest) {
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, false, false);
    std::vector<double> signal = {1.0, 1.5, 2.0, 3.0, 4.5, 6.0, 7.0, 11.0, 10.5, 13.0};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = {false, false, false, false, false, false, false, true, true, true};
    EXPECT_EQ(activations_expected, activations);
}

/* Test if the quick activation works */
TEST(HysteresisThresholdTest, QuickActivationTest) {
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, true, false);
    std::vector<double> signal = {1.0, 1.5, 2.0, 3.0, 4.5, 6.0, 7.0, 11.0, 10.5, 13.0};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = {false, false, false, false, false, true, true, true, true, true};
    EXPECT_EQ(activations_expected, activations);
}

/* Test if the deactivation works */
TEST(HysteresisThresholdTest, DeActivationTest) {
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, false, false);
    std::vector<double> signal = {14.0, 15.4, 8.0, 5.5, 9.5, 4.0, 1.0, 2.0, 1.5, 4.0};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = {true, true, true, true, true, false, false, false, false, false};
    EXPECT_EQ(activations_expected, activations);
}

/* Test if the quick deactivation works */
TEST(HysteresisThresholdTest, QuickDeActivationTest) {
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, false, true);
    std::vector<double> signal = {14.0, 15.4, 8.0, 5.5, 9.5, 4.0, 1.0, 2.0, 1.5, 4.0};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = {true, true, false, false, false, false, false, false, false, false};
    EXPECT_EQ(activations_expected, activations);
}


/* When the values don't go high enough to activate */
TEST(HysteresisThresholdTest, NotEnoughToActivateTest) {
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, false, false);
    std::vector<double> signal = {1.0, 1.5, 2.0, 3.0, 4.5, 6.0, 7.0, 8.0, 4.0, 3.0};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = {false, false, false, false, false, false, false, false, false, false};
    EXPECT_EQ(activations_expected, activations);
}

/* When the values don't go low enough to deactivate */
TEST(HysteresisThresholdTest, NotEnoughToDeActivateTest) {
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, false, false);
    std::vector<double> signal = {11.0, 11.5, 12.0, 13.0, 6.5, 6.0, 7.0, 11.0, 10.5, 13.0};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = {true, true, true, true, true, true, true, true, true, true};
    EXPECT_EQ(activations_expected, activations);
}

/* When there is only one sample deactivated */
TEST(HysteresisThresholdTest, OneSampleDeActivated) {
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, true, false, false);
    std::vector<double> signal = {11.0, 11.5, 12.0, 13.0, 16.5, 1.0, 17.0, 11.0, 10.5, 13.0};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = {false, false, false, false, false, true, false, false, false, false};
    EXPECT_EQ(activations_expected, activations);
}

/* When there is only one sample activated */
TEST(HysteresisThresholdTest, OneSampleActivated) {
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, false, false);
    std::vector<double> signal = {4.0, 1.4, 4.0, 3.5, 19.5, 4.0, 1.0, 2.0, 1.5, 4.0};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = {false, false, false, false, true, false, false, false, false, false};
    EXPECT_EQ(activations_expected, activations);
}

/* When the first value(s) are between both threshold */
TEST(HysteresisThresholdTest, StartingWithInBetweenValue) {
    // After : activated
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, false, false);
    std::vector<double> signal = {6.0, 6.5, 5.1, 13.0, 14.5, 11.0, 17.0, 14.0, 13.5, 13.0};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = {false, false, false, true, true, true, true, true, true, true};
    EXPECT_EQ(activations_expected, activations);
    // After : deactivated
    signal = {6.0, 6.5, 8.0, 3.0, 4.5, 6.0, 7.0, 8.0, 2.0, 0.0};
    activations = hysteresis_thresholding.perform(signal);
    activations_expected = {false, false, false, false, false, false, false, false, false, false};
    EXPECT_EQ(activations_expected, activations);
}

/* When the first value(s) are between both threshold with quick activation*/
TEST(HysteresisThresholdTest, StartingWithInBetweenValueWithQuickActivation) {
    // After : activated
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, true, false);
    std::vector<double> signal = {6.0, 6.5, 5.1, 13.0, 14.5, 11.0, 17.0, 14.0, 13.5, 13.0};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = {true, true, true, true, true, true, true, true, true, true};
    EXPECT_EQ(activations_expected, activations);
    // After : deactivated
    signal = {6.0, 6.5, 8.0, 3.0, 4.5, 6.0, 7.0, 8.0, 2.0, 0.0};
    activations = hysteresis_thresholding.perform(signal);
    activations_expected = {false, false, false, false, false, false, false, false, false, false};
    EXPECT_EQ(activations_expected, activations);
}

/* When the last value(s) are between both threshold */
TEST(HysteresisThresholdTest, EndingWithInBetweenValue) {
    // Before : activate
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, false, false);
    std::vector<double> signal = {11.0, 11.5, 12.0, 13.0, 14.5, 6.0, 7.0, 8.0, 9.0, 8.0};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = {true, true, true, true, true, true, true, true, true, true};
    EXPECT_EQ(activations_expected, activations);
    // Before : deactivate
    signal = {1.0, 1.5, 2.0, 3.0, 4.5, 6.0, 7.0, 8.0, 9.0, 8.0};
    activations = hysteresis_thresholding.perform(signal);
    activations_expected = {false, false, false, false, false, false, false, false, false, false};
    EXPECT_EQ(activations_expected, activations);
}

/* Testing mask mode */
TEST(HysteresisThresholdTest, MaskMode) {
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, true, true, false);
    std::vector<double> signal = {14.0, 15.4, 8.0, 5.5, 9.5, 4.0, 1.0, 2.0, 1.5, 4.0};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = {false, false, false, false, false, true, true, true, true, true};
    EXPECT_EQ(activations_expected, activations);
}

/* Only in between values */
TEST(HysteresisThresholdTest, OnlyInBetweenValues) {
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, true, false);
    std::vector<double> signal = {7.0, 6.5, 6.0, 7.0, 9.5, 6.0, 7.0, 9.0, 5.6, 8.0};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = {false, false, false, false, false, false, false, false, false, false};
    EXPECT_EQ(activations_expected, activations);
}

/* NaN values should be considered as below deactivation threshold */
TEST(HysteresisThresholdTest, NaNTest) {
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, true, false);
    double nan = std::numeric_limits<double>::quiet_NaN();
    std::vector<double> signal = {1.0, 1.5, nan, 3.0, 4.5, nan, nan, 11.0, nan, 13.0};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = {false, false, false, false, false, true, true, true, true, true};
    EXPECT_EQ(activations_expected, activations);
}

/* NaN values should be considered as below deactivation threshold */
TEST(HysteresisThresholdTest, NaNAtTheBeginningTest) {
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, false, false);
    double nan = std::numeric_limits<double>::quiet_NaN();
    std::vector<double> signal = {nan, 11.5, nan, 13.0, 4.5, 6.0, 7.0, 11.0, nan, 13.0};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = {false, true, true, true, false, false, false, true, true, true};
    EXPECT_EQ(activations_expected, activations);
}

/* NaN values should be considered as below deactivation threshold */
TEST(HysteresisThresholdTest, NaNAttheEndTest) {
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, true, false);
    double nan = std::numeric_limits<double>::quiet_NaN();
    std::vector<double> signal = {1.0, 1.5, nan, 3.0, 4.5, 6.0, 7.0, 11.0, nan, nan};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = {false, false, false, false, false, true, true, true, true, true};
    EXPECT_EQ(activations_expected, activations);
}

/* Empty vector */
TEST(HysteresisThresholdTest, EmptyVectorTest) {
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, true, false);
    std::vector<double> signal;
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected;
    EXPECT_EQ(activations_expected, activations);
}


/* Vector of One sample */
TEST(HysteresisThresholdTest, OneSampleTest) {
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, true, false);
    std::vector<double> signal = {15};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = { true };
    EXPECT_EQ(activations_expected, activations);
}

/* Vector of One sample */
TEST(HysteresisThresholdTest, OneSampleTest2) {
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, true, false);
    std::vector<double> signal = {2};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = { false };
    EXPECT_EQ(activations_expected, activations);
}

/* Vector of One sample */
TEST(HysteresisThresholdTest, OneSampleTest3) {
    HysteresisThreshold hysteresis_thresholding(5.0, 10.0, false, true, false);
    std::vector<double> signal = {7};
    std::vector<bool> activations = hysteresis_thresholding.perform(signal);
    std::vector<bool> activations_expected = { false };
    EXPECT_EQ(activations_expected, activations);
}
