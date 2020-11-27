#include <chrono>
#include <gtest/gtest.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include "PitchDetector.hpp"
#include "FFmpegAudioReader.hpp"
#include "McLeodPitchExtractorMethod.hpp"

TEST(PitchDetectorTest, Basic) {
    std::string filepath = "../../../tests/sound_samples/440Hz_44100Hz_16bit_05sec.wav";
    AudioReaderParameters reader_parameters;
    reader_parameters.windowstimesize_s = 20e-3;
    reader_parameters.period_s = 1e-3;
    reader_parameters.resample_rate_hz = 44100;
    McLeodParameters mcleod_parameters;
    mcleod_parameters.cutoff = 0.97;
    mcleod_parameters.small_cutoff = 0.5;
    mcleod_parameters.lower_pitch_cutoff = 50.0;
    // Audio reader
    FFmpegAudioReader audio_reader(filepath);
    // Pitch extractor method
    McLeodPitchExtractorMethod mcleod_method(mcleod_parameters);
    // Pitch detector
    PitchDetector pitch_detector(&audio_reader, &mcleod_method);
    auto tstart = std::chrono::high_resolution_clock::now();
    PitchResult result = pitch_detector.perform(NULL, reader_parameters);
    auto tend = std::chrono::high_resolution_clock::now();
    auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tend-tstart).count();
    std::cout << "Processing time: " << time_ms << " ms" << std::endl;
    EXPECT_EQ(result.period_s, reader_parameters.period_s);
    EXPECT_TRUE(result.energy.size() > 0);
    EXPECT_TRUE(result.pitch_st.size() > 0);
    bool good;
    for(auto const& value: result.pitch_st) {
        good = std::abs(value - 45) < 0.5;
        EXPECT_TRUE(good);
        if(!good) {
            break;
        }
    }
}
