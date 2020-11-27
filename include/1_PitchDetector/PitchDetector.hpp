#ifndef PITCH_DETECTOR
#define PITCH_DETECTOR

#include <thread>
#include <future>
#include <atomic>
#include "AudioReader.hpp"
#include "PitchExtractorMethod.hpp"
#include "common_tools.hpp"


struct PitchResult {
    double period_s;
    double f0_hz;
    double offset_s;
    std::vector<double> pitch_st;
    std::vector<double> energy;
};

const unsigned int CONCURRENT_THREADS_SUPPORTED = std::thread::hardware_concurrency();

class PitchDetector {
public:
    // Constructor
    PitchDetector(AudioReader *audio_reader, PitchExtractorMethod *pitch_extractor);
    // Destructor
    ~PitchDetector();
    // Performing Pitch extraction
    PitchResult perform(std::atomic<float> * progress,
                        const AudioReaderParameters & audio_parameters=DEFAULT_AUDIO_READER_PARAMETERS,
                        unsigned int audio_stream_ind=0,
                        double timestart_s=-1.0,
                        double timestop_s=-1.0);
    void setFO(double f0_hz);
    double getFO();
private:
    PitchResult tempresult;
    double f0_hz;
    AudioReader *audio_reader;
    PitchExtractorMethod *pitch_extractor;
    // Calculate the normalized energy of an audio buffer
    void processBuffer(size_t index, double sample_rate_hz, const std::vector<double> buffer);
    double getEnergy(const std::vector<double> & audio_buffer) const;
};

#endif /* PITCH_DETECTOR */
