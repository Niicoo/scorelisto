#include <gtest/gtest.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include "HistogramStepDetector.hpp"


TEST(compareByLengthTest, Basic) {
    DetectedPitch pitch_A = {3.0, 78.0};
    DetectedPitch pitch_B1 = {3.0, 45.0};
    DetectedPitch pitch_B2 = {3.0, 88.0};
    DetectedPitch pitch_C1 = {1.0, 33.0};
    DetectedPitch pitch_C2 = {1.0, 89.0};
    DetectedPitch pitch_D1 = {5.0, 34.0};
    DetectedPitch pitch_D2 = {5.0, 99.0};
    EXPECT_FALSE(compareByLength(pitch_A, pitch_B1));
    EXPECT_FALSE(compareByLength(pitch_A, pitch_B2));
    EXPECT_FALSE(compareByLength(pitch_A, pitch_C1));
    EXPECT_FALSE(compareByLength(pitch_A, pitch_C2));
    EXPECT_TRUE(compareByLength(pitch_A, pitch_D1));
    EXPECT_TRUE(compareByLength(pitch_A, pitch_D2));
}

TEST(compareByPitchTest, Basic) {
    DetectedPitch pitch_A = {4.0, 25.0};
    DetectedPitch pitch_B1 = {3.0, 25.0};
    DetectedPitch pitch_B2 = {5.0, 25.0};
    DetectedPitch pitch_C1 = {1.0, 15.0};
    DetectedPitch pitch_C2 = {6.0, 15.0};
    DetectedPitch pitch_D1 = {3.0, 40.0};
    DetectedPitch pitch_D2 = {5.0, 40.0};
    EXPECT_FALSE(compareByPitch(pitch_A, pitch_B1));
    EXPECT_FALSE(compareByPitch(pitch_A, pitch_B2));
    EXPECT_FALSE(compareByPitch(pitch_A, pitch_C1));
    EXPECT_FALSE(compareByPitch(pitch_A, pitch_C2));
    EXPECT_TRUE(compareByPitch(pitch_A, pitch_D1));
    EXPECT_TRUE(compareByPitch(pitch_A, pitch_D2));
}

TEST(HistogramStepDetectorTest, OneNote) {
    // Parameters
    StepParameters parameters;
    parameters.min_note_length_s = 50e-3;
    parameters.min_note_gap_st = 2/3.0;
    parameters.median_filter_width_s = 20e-3;
    parameters.min_pitch_st = 0.0;
    parameters.max_pitch_st = 200.0;
    // Step detector
    PitchResult pitch_result;
    pitch_result.period_s = 10e-3;
    pitch_result.f0_hz = 32.7032;
    pitch_result.pitch_st = {4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0};
    pitch_result.energy = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
    HistogramStepDetector step_detector(pitch_result);
    StepResult result = step_detector.perform(nullptr, parameters);
    EXPECT_EQ(result.offset_s, 0.0);
    EXPECT_EQ(result.notes.size(), 1);
    // Note 1
    EXPECT_TRUE(result.notes[0].is_a_note);
    EXPECT_EQ(result.notes[0].length_s, 80e-3);
    EXPECT_FALSE(result.notes[0].linked);
    EXPECT_TRUE(abs(result.notes[0].pitch_st - 4.0) < 0.01);
    EXPECT_TRUE(abs(result.notes[0].energy - 0.1) < 0.001);
}



TEST(HistogramStepDetectorTest, TwoNotes) {
    // Parameters
    StepParameters parameters;
    parameters.min_note_length_s = 50e-3;
    parameters.min_note_gap_st = 2/3.0;
    parameters.median_filter_width_s = 20e-3;
    parameters.min_pitch_st = 0.0;
    parameters.max_pitch_st = 200.0;
    // Step detector
    PitchResult pitch_result;
    pitch_result.period_s = 10e-3;
    pitch_result.f0_hz = 32.7032;
    pitch_result.pitch_st = {3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0};
    HistogramStepDetector step_detector(pitch_result);
    StepResult result = step_detector.perform(nullptr, parameters);
    EXPECT_EQ(result.offset_s, 0.0);
    EXPECT_EQ(result.notes.size(), 2);
    // Note 1
    EXPECT_TRUE(result.notes[0].is_a_note);
    EXPECT_EQ(result.notes[0].length_s, 100e-3);
    EXPECT_FALSE(result.notes[0].linked);
    EXPECT_TRUE(abs(result.notes[0].pitch_st - 3.0) < 0.01);
    EXPECT_TRUE(std::isnan(result.notes[0].energy));
    // Note 2
    EXPECT_TRUE(result.notes[1].is_a_note);
    EXPECT_EQ(result.notes[1].length_s, 100e-3);
    EXPECT_TRUE(result.notes[1].linked);
    EXPECT_TRUE(abs(result.notes[1].pitch_st - 5.0) < 0.01);
    EXPECT_TRUE(std::isnan(result.notes[1].energy));
}


TEST(HistogramStepDetectorTest, OneShortNan) {
    double nan = std::numeric_limits<double>::quiet_NaN();
    // Parameters
    StepParameters parameters;
    parameters.min_note_length_s = 50e-3;
    parameters.min_note_gap_st = 2/3.0;
    parameters.median_filter_width_s = 20e-3;
    parameters.min_pitch_st = 0.0;
    parameters.max_pitch_st = 200.0;
    // Step detector
    PitchResult pitch_result;
    pitch_result.period_s = 10e-3;
    pitch_result.f0_hz = 32.7032;
    pitch_result.pitch_st = {3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, nan, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0};
    HistogramStepDetector step_detector(pitch_result);
    StepResult result = step_detector.perform(nullptr, parameters);
    EXPECT_EQ(result.offset_s, 0.0);
    EXPECT_EQ(result.notes.size(), 2);
    // Note 1
    EXPECT_TRUE(result.notes[0].is_a_note);
    EXPECT_EQ(result.notes[0].length_s, 110e-3);
    EXPECT_FALSE(result.notes[0].linked);
    EXPECT_TRUE(abs(result.notes[0].pitch_st - 3.0) < 0.01);
    EXPECT_TRUE(std::isnan(result.notes[0].energy));
    // Note 2
    EXPECT_TRUE(result.notes[1].is_a_note);
    EXPECT_EQ(result.notes[1].length_s, 100e-3);
    EXPECT_TRUE(result.notes[1].linked);
    EXPECT_TRUE(abs(result.notes[1].pitch_st - 5.0) < 0.01);
    EXPECT_TRUE(std::isnan(result.notes[1].energy));
}


TEST(HistogramStepDetectorTest, OneLongNan) {
    double nan = std::numeric_limits<double>::quiet_NaN();
    // Parameters
    StepParameters parameters;
    parameters.min_note_length_s = 50e-3;
    parameters.min_note_gap_st = 2/3.0;
    parameters.median_filter_width_s = 30e-3;
    parameters.min_pitch_st = 0.0;
    parameters.max_pitch_st = 200.0;
    // Step detector
    PitchResult pitch_result;
    pitch_result.period_s = 10e-3;
    pitch_result.f0_hz = 32.7032;
    pitch_result.pitch_st = {3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, nan, nan, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0};
    HistogramStepDetector step_detector(pitch_result);
    StepResult result = step_detector.perform(nullptr, parameters);
    EXPECT_EQ(result.offset_s, 0.0);
    EXPECT_EQ(result.notes.size(), 3);
    // Note 1
    EXPECT_TRUE(result.notes[0].is_a_note);
    EXPECT_EQ(result.notes[0].length_s, 100e-3);
    EXPECT_FALSE(result.notes[0].linked);
    EXPECT_TRUE(abs(result.notes[0].pitch_st - 3.0) < 0.01);
    EXPECT_TRUE(std::isnan(result.notes[0].energy));
    // Note 2
    EXPECT_FALSE(result.notes[1].is_a_note);
    EXPECT_EQ(result.notes[1].length_s, 20e-3);
    EXPECT_FALSE(result.notes[1].linked);
    EXPECT_TRUE(std::isnan(result.notes[1].pitch_st));
    EXPECT_TRUE(std::isnan(result.notes[1].energy));
    // Note 3
    EXPECT_TRUE(result.notes[2].is_a_note);
    EXPECT_EQ(result.notes[2].length_s, 100e-3);
    EXPECT_FALSE(result.notes[2].linked);
    EXPECT_TRUE(abs(result.notes[2].pitch_st - 5.0) < 0.01);
    EXPECT_TRUE(std::isnan(result.notes[2].energy));
}



TEST(HistogramStepDetectorTest, NoteTooShort) {
    // Parameters
    StepParameters parameters;
    parameters.min_note_length_s = 50e-3;
    parameters.min_note_gap_st = 2/3.0;
    parameters.median_filter_width_s = 20e-3;
    parameters.min_pitch_st = 0.0;
    parameters.max_pitch_st = 200.0;
    // Step detector
    PitchResult pitch_result;
    pitch_result.period_s = 10e-3;
    pitch_result.f0_hz = 32.7032;
    pitch_result.pitch_st = {3.0, 3.0, 6.0, 6.0, 6.0, 6.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0};
    HistogramStepDetector step_detector(pitch_result);
    StepResult result = step_detector.perform(nullptr, parameters);
    EXPECT_EQ(result.offset_s, 0.0);
    EXPECT_EQ(result.notes.size(), 2);
    // Note 1
    EXPECT_TRUE(result.notes[0].is_a_note);
    EXPECT_EQ(result.notes[0].length_s, 130e-3);
    EXPECT_FALSE(result.notes[0].linked);
    EXPECT_TRUE(abs(result.notes[0].pitch_st - 3.0) < 0.01);
    EXPECT_TRUE(std::isnan(result.notes[0].energy));
    // Note 2
    EXPECT_TRUE(result.notes[1].is_a_note);
    EXPECT_EQ(result.notes[1].length_s, 100e-3);
    EXPECT_TRUE(result.notes[1].linked);
    EXPECT_TRUE(abs(result.notes[1].pitch_st - 5.0) < 0.01);
    EXPECT_TRUE(std::isnan(result.notes[1].energy));
}


TEST(HistogramStepDetectorTest, NoteTooClose) {
    // Parameters
    StepParameters parameters;
    parameters.min_note_length_s = 50e-3;
    parameters.min_note_gap_st = 2/3.0;
    parameters.median_filter_width_s = 20e-3;
    parameters.min_pitch_st = 0.0;
    parameters.max_pitch_st = 200.0;
    // Step detector
    PitchResult pitch_result;
    pitch_result.period_s = 10e-3;
    pitch_result.f0_hz = 32.7032;
    pitch_result.pitch_st = {3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.5, 3.5, 3.5, 3.5, 3.5, 3.5, 3.5, 3.5, 3.5, 3.5};
    HistogramStepDetector step_detector(pitch_result);
    StepResult result = step_detector.perform(nullptr, parameters);
    EXPECT_EQ(result.offset_s, 0.0);
    EXPECT_EQ(result.notes.size(), 1);
    // Note 1
    EXPECT_TRUE(result.notes[0].is_a_note);
    EXPECT_EQ(result.notes[0].length_s, 200e-3);
    EXPECT_FALSE(result.notes[0].linked);
    EXPECT_TRUE(abs(result.notes[0].pitch_st - 3.25) < 0.01);
    EXPECT_TRUE(std::isnan(result.notes[0].energy));
}


TEST(HistogramStepDetectorTest, InputMask) {
    // Parameters
    StepParameters parameters;
    parameters.min_note_length_s = 50e-3;
    parameters.min_note_gap_st = 2/3.0;
    parameters.median_filter_width_s = 20e-3;
    parameters.min_pitch_st = 0.0;
    parameters.max_pitch_st = 200.0;
    // Step detector
    PitchResult pitch_result;
    pitch_result.period_s = 10e-3;
    pitch_result.f0_hz = 32.7032;
    pitch_result.pitch_st = {3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0};
    std::vector<bool> mask = {false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false, false};
    HistogramStepDetector step_detector(pitch_result, mask);
    StepResult result = step_detector.perform(nullptr, parameters);
    EXPECT_EQ(result.offset_s, 0.0);
    EXPECT_EQ(result.notes.size(), 3);
    // Note 1
    EXPECT_TRUE(result.notes[0].is_a_note);
    EXPECT_EQ(result.notes[0].length_s, 90e-3);
    EXPECT_FALSE(result.notes[0].linked);
    EXPECT_TRUE(abs(result.notes[0].pitch_st - 3.0) < 0.01);
    EXPECT_TRUE(std::isnan(result.notes[0].energy));
    // Note 2
    EXPECT_FALSE(result.notes[1].is_a_note);
    EXPECT_EQ(result.notes[1].length_s, 10e-3);
    EXPECT_FALSE(result.notes[1].linked);
    EXPECT_TRUE(std::isnan(result.notes[1].pitch_st));
    EXPECT_TRUE(std::isnan(result.notes[1].energy));
    // Note 3
    EXPECT_TRUE(result.notes[2].is_a_note);
    EXPECT_EQ(result.notes[2].length_s, 90e-3);
    EXPECT_FALSE(result.notes[2].linked);
    EXPECT_TRUE(abs(result.notes[2].pitch_st - 3.0) < 0.01);
    EXPECT_TRUE(std::isnan(result.notes[2].energy));
}

