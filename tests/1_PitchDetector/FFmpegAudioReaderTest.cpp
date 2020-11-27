#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>
#include "FFmpegAudioReader.hpp"
#include "McLeodPitchExtractorMethod.hpp"



void ExtractBuffers(FFmpegAudioReader &audio_reader) {
    int64_t rate_hz = audio_reader.getOutputSampleRate();
    double windowsize_s = audio_reader.getOutputWindowSize();
    double period_s = audio_reader.getOuputPeriod();
    // size of the window
    int64_t buffer_size = (int64_t)round(windowsize_s*(double)rate_hz);
    unsigned int nb_samples = 0;
    std::vector<double> temp_buffer(1, 0);
    while(audio_reader.getNextBuffer(temp_buffer)) {
        EXPECT_EQ(temp_buffer.size(), buffer_size);
        nb_samples++;
    }
    int64_t nb_samples_expected = (int64_t)floor((audio_reader.getStreams()[0].duration_s - windowsize_s)/ period_s);
    EXPECT_LE(abs(nb_samples-nb_samples_expected)*period_s, 100e-3);
}


TEST(AudioReaderTest, WavAudioFile) {
    // Input audio file
    std::string filepath = "../../../tests/sound_samples/whistling_stereo.wav";
    FFmpegAudioReader audio_reader(filepath);
    // Initialize the parameters for extraction
    AudioReaderParameters parameters;
    parameters.windowstimesize_s = 20e-3;
    parameters.period_s = 1e-3;
    parameters.resample_rate_hz = 44100;
    audio_reader.initExtraction(parameters);
    ExtractBuffers(audio_reader);
}

TEST(AudioReaderTest, OggAudioFile) {
    // Input audio file
    std::string filepath = "../../../tests/sound_samples/whistling_stereo.ogg";
    FFmpegAudioReader audio_reader(filepath);
    // Initialize the parameters for extraction
    AudioReaderParameters parameters;
    parameters.windowstimesize_s = 20e-3;
    parameters.period_s = 1e-3;
    parameters.resample_rate_hz = 22050;
    audio_reader.initExtraction(parameters);
    ExtractBuffers(audio_reader);
}

TEST(AudioReaderTest, Mp3AudioFile) {
    // Input audio file
    std::string filepath = "../../../tests/sound_samples/whistling_stereo.mp3";
    FFmpegAudioReader audio_reader(filepath);
    // Initialize the parameters for extraction
    AudioReaderParameters parameters;
    parameters.windowstimesize_s = 20e-3;
    parameters.period_s = 1e-3;
    parameters.resample_rate_hz = -1;
    audio_reader.initExtraction(parameters);
    ExtractBuffers(audio_reader);
}

TEST(AudioReaderTest, FlacAudioFile) {
    // Input audio file
    std::string filepath = "../../../tests/sound_samples/whistling_stereo.flac";
    FFmpegAudioReader audio_reader(filepath);
    // Initialize the parameters for extraction
    AudioReaderParameters parameters;
    parameters.windowstimesize_s = 50e-3;
    parameters.period_s = 1e-3;
    parameters.resample_rate_hz = 8000;
    audio_reader.initExtraction(parameters);
    ExtractBuffers(audio_reader);
}

TEST(AudioReaderTest, Mp3MonoAudioFile) {
    // Input audio file
    std::string filepath = "../../../tests/sound_samples/whistling_mono.mp3";
    FFmpegAudioReader audio_reader(filepath);
    // Initialize the parameters for extraction
    AudioReaderParameters parameters;
    parameters.windowstimesize_s = 40e-3;
    parameters.period_s = 3e-3;
    parameters.resample_rate_hz = 48000;
    audio_reader.initExtraction(parameters);
    ExtractBuffers(audio_reader);
}

TEST(AudioReaderTest, Ogg5ChannelsAudioFile) {
    // Input audio file
    std::string filepath = "../../../tests/sound_samples/whistling_5_channels.ogg";
    FFmpegAudioReader audio_reader(filepath);
    // Initialize the parameters for extraction
    AudioReaderParameters parameters;
    parameters.windowstimesize_s = 200e-3;
    parameters.period_s = 10e-3;
    parameters.resample_rate_hz = 44100;
    audio_reader.initExtraction(parameters);
    ExtractBuffers(audio_reader);
}

TEST(AudioReaderTest, Ac35_1AudioFile) {
    // Input audio file
    std::string filepath = "../../../tests/sound_samples/5_1_sound.ac3";
    FFmpegAudioReader audio_reader(filepath);
    // Initialize the parameters for extraction
    AudioReaderParameters parameters;
    parameters.windowstimesize_s = 20e-3;
    parameters.period_s = 1e-3;
    parameters.resample_rate_hz = 44100;
    audio_reader.initExtraction(parameters, 0);
    ExtractBuffers(audio_reader);
}
