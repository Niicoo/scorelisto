#include <gtest/gtest.h>
#include "CumulativeSumThreshold.hpp"




/* Test if the activation works */
TEST(CumulativeSumThresholdTest, ActivationTest) {
    double threshold_min = 1.0;
    double cumsum_min = 3.0;
    bool mode_mask = false;
    CumulativeSumThreshold cumsum_thresholding(threshold_min, cumsum_min, mode_mask);
    std::vector<double> signal = {0.5, 0.5, 0.3, 0.2, 1.5, 1.1, 1.1, 2.0, 0.8, 0.7};
    std::vector<bool> activations = cumsum_thresholding.perform(signal);
    std::vector<bool> activations_expected = {false, false, false, false, true, true, true, true, false, false};
    EXPECT_EQ(activations_expected, activations);
}


/* Test if the mode mask works */
TEST(CumulativeSumThresholdTest, ModeMask) {
    double threshold_min = 1.0;
    double cumsum_min = 3.0;
    bool mode_mask = true;
    CumulativeSumThreshold cumsum_thresholding(threshold_min, cumsum_min, mode_mask);
    std::vector<double> signal = {0.5, 0.5, 0.3, 0.2, 1.5, 1.1, 1.1, 2.0, 0.8, 0.7};
    std::vector<bool> activations = cumsum_thresholding.perform(signal);
    std::vector<bool> activations_expected = {true, true, true, true, false, false, false, false, true, true};
    EXPECT_EQ(activations_expected, activations);
}


/* Test if the activation not working if not enough cumulative sum reached */
TEST(CumulativeSumThresholdTest, CumSumNotReached) {
    double threshold_min = 1.0;
    double cumsum_min = 3.0;
    bool mode_mask = false;
    CumulativeSumThreshold cumsum_thresholding(threshold_min, cumsum_min, mode_mask);
    std::vector<double> signal = {0.5, 0.5, 0.3, 0.2, 1.5, 1.1, 0.8, 2.0, 0.8, 0.7};
    std::vector<bool> activations = cumsum_thresholding.perform(signal);
    std::vector<bool> activations_expected = {false, false, false, false, false, false, false, false, false, false};
    EXPECT_EQ(activations_expected, activations);
}


/* Test activation from the beginning */
TEST(CumulativeSumThresholdTest, ActiveAtTheBeginning) {
    double threshold_min = 1.0;
    double cumsum_min = 3.0;
    bool mode_mask = false;
    CumulativeSumThreshold cumsum_thresholding(threshold_min, cumsum_min, mode_mask);
    std::vector<double> signal = {1.5, 1.5, 1.3, 0.2, 2.0, 2.1, 0.8, 2.0, 0.8, 0.7};
    std::vector<bool> activations = cumsum_thresholding.perform(signal);
    std::vector<bool> activations_expected = {true, true, true, false, true, true, false, false, false, false};
    EXPECT_EQ(activations_expected, activations);
}


/* Test activation with only one sample */
TEST(CumulativeSumThresholdTest, ActiveWithOnlyOneSample) {
    double threshold_min = 1.0;
    double cumsum_min = 3.0;
    bool mode_mask = false;
    CumulativeSumThreshold cumsum_thresholding(threshold_min, cumsum_min, mode_mask);
    std::vector<double> signal = {4.5, 0.5, 0.3, 0.2, 5.0, 0.85, 0.8, 0.9, 0.8, 6.7};
    std::vector<bool> activations = cumsum_thresholding.perform(signal);
    std::vector<bool> activations_expected = {true, false, false, false, true, false, false, false, false, true};
    EXPECT_EQ(activations_expected, activations);
}


/* Test with value below activation at the very beginning */
TEST(CumulativeSumThresholdTest, DeactivatedAttheFirstSample) {
    double threshold_min = 1.0;
    double cumsum_min = 3.0;
    bool mode_mask = false;
    CumulativeSumThreshold cumsum_thresholding(threshold_min, cumsum_min, mode_mask);
    std::vector<double> signal = {0.5, 5.5, 5.3, 4.2, 4.0, 3.85, 2.8, 6.9, 6.8, 6.7};
    std::vector<bool> activations = cumsum_thresholding.perform(signal);
    std::vector<bool> activations_expected = {false, true, true, true, true, true, true, true, true, true};
    EXPECT_EQ(activations_expected, activations);
}


/* Test with empty vector */
TEST(CumulativeSumThresholdTest, EmptyVectorTest) {
    double threshold_min = 1.0;
    double cumsum_min = 3.0;
    bool mode_mask = false;
    CumulativeSumThreshold cumsum_thresholding(threshold_min, cumsum_min, mode_mask);
    std::vector<double> signal;
    std::vector<bool> activations = cumsum_thresholding.perform(signal);
    std::vector<bool> activations_expected;
    EXPECT_EQ(activations_expected, activations);
}


/* Test with one sample only */
TEST(CumulativeSumThresholdTest, OneSampleTest1) {
    double threshold_min = 1.0;
    double cumsum_min = 3.0;
    bool mode_mask = false;
    CumulativeSumThreshold cumsum_thresholding(threshold_min, cumsum_min, mode_mask);
    std::vector<double> signal = {15};
    std::vector<bool> activations = cumsum_thresholding.perform(signal);
    std::vector<bool> activations_expected = { true };
    EXPECT_EQ(activations_expected, activations);
}

/* Test with one sample only */
TEST(CumulativeSumThresholdTest, OneSampleTest2) {
    double threshold_min = 1.0;
    double cumsum_min = 3.0;
    bool mode_mask = false;
    CumulativeSumThreshold cumsum_thresholding(threshold_min, cumsum_min, mode_mask);
    std::vector<double> signal = { 0.5 };
    std::vector<bool> activations = cumsum_thresholding.perform(signal);
    std::vector<bool> activations_expected = { false };
    EXPECT_EQ(activations_expected, activations);
}


/* Test with one sample only */
TEST(CumulativeSumThresholdTest, OneSampleTest3) {
    double threshold_min = 1.0;
    double cumsum_min = 3.0;
    bool mode_mask = false;
    CumulativeSumThreshold cumsum_thresholding(threshold_min, cumsum_min, mode_mask);
    std::vector<double> signal = { 2.5 };
    std::vector<bool> activations = cumsum_thresholding.perform(signal);
    std::vector<bool> activations_expected = { false };
    EXPECT_EQ(activations_expected, activations);
}
