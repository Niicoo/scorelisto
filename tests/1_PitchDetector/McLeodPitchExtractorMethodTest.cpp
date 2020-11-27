#include <gtest/gtest.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include "McLeodPitchExtractorMethod.hpp"

TEST(McLeodPitchExtractorMethodTest, CorrectExtractedFrequency) {
    // Declare a pitch extractor method
    McLeodPitchExtractorMethod mcleod_method;
    // Create a pure sinus
    const double pi = 3.14159265358979323846;
    unsigned int N = 1000;
    double sample_rate_hz = 44100;
    std::vector<double> frequencies = {100.0, 200.0, 500.0, 1000.0, 2000.0, 4000.0};
    std::vector<double> audio_buffer(N);
    double freq_estimated;
    for(auto &freq : frequencies) {
        // Create temporary signal at desired frequency
        for(unsigned int k=0; k< audio_buffer.size(); k++) {
            audio_buffer[k] = sin(2*pi*freq*k/sample_rate_hz);
        }
        // Estimate frequency
        freq_estimated = mcleod_method.get_pitch(audio_buffer, sample_rate_hz);
        EXPECT_TRUE(std::abs(freq_estimated - freq) < 3);
    }
}
