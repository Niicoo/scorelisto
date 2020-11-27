#ifndef AUDIO_READER
#define AUDIO_READER

#include <string>
#include <iostream>
#include <vector>
#include "PitchExtractorMethod.hpp"


// Information relative to an audio stream of a multimedia file
struct AudioStream {
    unsigned int ind_stream;
    std::string format;
    unsigned int nb_channels;
    unsigned long int nb_samples;
    int64_t sample_rate_hz;
    double duration_s;
    unsigned long int bit_rate; // byte
};


// Parameters needed to prepare extraction of audio buffers
struct AudioReaderParameters {
    double windowstimesize_s;
    double period_s;
    int64_t resample_rate_hz;
};
const AudioReaderParameters DEFAULT_AUDIO_READER_PARAMETERS = {20e-3, 1e-3, -1};


class AudioReader {
public:
    AudioReader(std::string filepath);
    virtual ~AudioReader();
    std::string getFilePath() const;
    std::vector<AudioStream> getStreams() const;
    unsigned int getNumberOfStream() const;
    double getOuputPeriod() const;
    int64_t getOutputSampleRate() const;
    double getOutputWindowSize() const;
    // Set parameters for buffer extraction 
    virtual void initExtraction(const AudioReaderParameters & parameters=DEFAULT_AUDIO_READER_PARAMETERS,
                                unsigned int audio_stream_ind=0,
                                double timestart_s=-1.0,
                                double timestop_s=-1.0) = 0;
    // Get the next buffer of audio: initExtraction() must be called before calling this function
    virtual bool getNextBuffer(std::vector<double> & buffer) = 0;
    void showStreamsInfos(std::ostream & stream_out) const;
protected:
    // Path of the multimedia file
    std::string filepath;
    std::vector<AudioStream> streams;
    double dst_period_s;
    int64_t dst_rate_hz;
    double dst_windowsize_s;
};

#endif /* AUDIO_READER */
