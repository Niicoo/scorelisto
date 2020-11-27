#include "AudioReader.hpp"
#include "common_tools.hpp"
#include <iostream>
#include <cmath>
#include <limits>

AudioReader::AudioReader(std::string filepath) {
    // Constructor
    this->filepath = filepath;
    // this->pitch_extractor_ptr = extractor;
    // Check if the audio file exists
    if(!exists(this->filepath)) {
        throw std::runtime_error("Could not open file");
    }
}


std::string AudioReader::getFilePath() const {
    return this->filepath;
}


std::vector<AudioStream> AudioReader::getStreams() const {
    return this->streams;
}


unsigned int AudioReader::getNumberOfStream() const {
    return this->streams.size();
}


AudioReader::~AudioReader() {
    // Destructor
}


void AudioReader::showStreamsInfos(std::ostream & stream_out) const{
    stream_out << "[AUDIO_STREAM_INFOS]" << std::endl ;
    for (unsigned int k=0; k < this->getNumberOfStream(); k++) {
        stream_out << "[STREAM NO " << k << "]" << std::endl ;
        stream_out << "Number of audio stream:" << this->getStreams().size() << std::endl ;
        stream_out << "Indice of the stream: " << this->getStreams()[k].ind_stream << std::endl ;
        stream_out << "Format: " << this->getStreams()[k].format << std::endl ;
        stream_out << "Sample rate: " << this->getStreams()[k].sample_rate_hz << std::endl ;
        stream_out << "Number of channels: " << this->getStreams()[k].nb_channels << std::endl ;
        stream_out << "Number of samples: " << this->getStreams()[k].nb_samples << std::endl ;
        stream_out << "duration: " << this->getStreams()[k].duration_s << std::endl ;
        stream_out << "Bitrate: " << this->getStreams()[k].bit_rate << std::endl ;
    }
    stream_out << "[/AUDIO_STREAM_INFOS]" << std::endl ;
}


double AudioReader::getOuputPeriod() const{
    return this->dst_period_s;
}


int64_t AudioReader::getOutputSampleRate() const{
    return this->dst_rate_hz;
}


double AudioReader::getOutputWindowSize() const{
    return this->dst_windowsize_s;
}
