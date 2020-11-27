#include <thread>
#include <exception>
#include <chrono>
#include <future>
#include <cmath>
#include <iostream>
#include "PitchDetector.hpp"
#include "AudioReader.hpp"
#include "PitchExtractorMethod.hpp"
#include "common_tools.hpp"


PitchDetector::PitchDetector(AudioReader *audio_reader, PitchExtractorMethod *pitch_extractor) {
    // Constructor
    this->audio_reader = audio_reader;
    this->pitch_extractor = pitch_extractor;
    this->f0_hz = F0_HZ;
}


PitchDetector::~PitchDetector() {
    // Destructor
}


double PitchDetector::getEnergy(const std::vector<double> & audio_buffer) const {
    // audio_buffer must be between -1 and 1 if I want consistent result whatever input format
    double energy = 0;
    for(const auto & sample : audio_buffer) {
        energy += sample * sample;
    }
    energy = energy / audio_buffer.size();
    return energy;
}


template<typename T>
bool future_is_ready(std::future<T>& t){
    return t.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

void remove_futures_ready(std::vector<std::future<void>> & async_ret) {
    for(int64_t k = async_ret.size()-1; k >= 0; k--) {
        if(future_is_ready(async_ret[k])) {
            async_ret.erase(async_ret.begin() + k);
        }
    }
}

PitchResult PitchDetector::perform( std::atomic<float> * progress,
                                    const AudioReaderParameters & audio_parameters/*=DEFAULT_AUDIO_READER_PARAMETERS*/,
                                    unsigned int audio_stream_ind/*=0*/,
                                    double timestart_s/*=-1.0*/,
                                    double timestop_s/*=-1.0*/
                                    ) {
    this->tempresult.period_s = audio_parameters.period_s;
    this->tempresult.f0_hz = this->f0_hz;
    this->tempresult.offset_s = (timestart_s < 0.0) ? 0.0 : timestart_s;
    this->tempresult.pitch_st.clear();
    this->tempresult.energy.clear();
    this->audio_reader->initExtraction(audio_parameters, audio_stream_ind, timestart_s, timestop_s);
    double dst_sample_rate_hz = (double)this->audio_reader->getOutputSampleRate();
    double duration_s = this->audio_reader->getStreams()[audio_stream_ind].duration_s;
    double nan = std::numeric_limits<double>::quiet_NaN();
    // Temporary variables
    std::vector<double> temp_buffer;
    unsigned int nb_thread_free = std::max((unsigned int)1, CONCURRENT_THREADS_SUPPORTED);
    std::vector<std::future<void>> async_ret;
    uint64_t k = 0;
    while(this->audio_reader->getNextBuffer(temp_buffer)) {
        this->tempresult.pitch_st.push_back(nan);
        this->tempresult.energy.push_back(nan);
        while(async_ret.size() >= CONCURRENT_THREADS_SUPPORTED) {
            remove_futures_ready(async_ret);
        }
        async_ret.push_back(std::async(std::launch::async, &PitchDetector::processBuffer, this, k, dst_sample_rate_hz, temp_buffer));
        k += 1;
        if(progress->load() < 0) {
            while(async_ret.size() >= CONCURRENT_THREADS_SUPPORTED) {
                remove_futures_ready(async_ret);
            }
            throw std::runtime_error("Process cancel by the user");
        }
        *progress = (audio_parameters.period_s * this->tempresult.pitch_st.size() * 100.0) / duration_s;
    }
    *progress = 100.0;
    return this->tempresult;
}


void PitchDetector::processBuffer(size_t index, double sample_rate_hz, const std::vector<double> buffer) {
    double pitch_hz = this->pitch_extractor->get_pitch(buffer, sample_rate_hz);
    double pitch_st = convert_freq_to_tone(pitch_hz, this->tempresult.f0_hz);
    double energy = this->getEnergy(buffer);
    this->tempresult.energy[index] = energy;
    this->tempresult.pitch_st[index] = pitch_st;
}

// #include <cmath>
// #include <iostream>
// #include "PitchDetector.hpp"
// #include "AudioReader.hpp"
// #include "PitchExtractorMethod.hpp"
// #include "common_tools.hpp"
// #include <atomic>
// // Mutliprocessing
// #ifdef __unix__
// #include <unistd.h>
// #include <sys/wait.h>
// #endif


// PitchDetector::PitchDetector(AudioReader *audio_reader, PitchExtractorMethod *pitch_extractor) {
//     // Constructor
//     this->audio_reader = audio_reader;
//     this->pitch_extractor = pitch_extractor;
//     this->f0_hz = F0_HZ;
// }


// PitchDetector::~PitchDetector() {
//     // Destructor
// }


// double PitchDetector::getEnergy(const std::vector<double> & audio_buffer) const {
//     // audio_buffer must be between -1 and 1 if I want consistent result whatever input format
//     double energy = 0;
//     for(const auto & sample : audio_buffer) {
//         energy += sample * sample;
//     }
//     energy = energy / audio_buffer.size();
//     return energy;
// }



// #ifdef __unix__
// typedef std::tuple<uint64_t, double, double> pipe_element;
// const int PIPE_READ = 0;
// const int PIPE_WRITE = 1;

// PitchResult PitchDetector::perform( std::atomic<float> * progress,
//                                     const AudioReaderParameters & audio_parameters/*=DEFAULT_AUDIO_READER_PARAMETERS*/,
//                                     unsigned int audio_stream_ind/*=0*/,
//                                     double timestart_s/*=-1.0*/,
//                                     double timestop_s/*=-1.0*/
//                                     ) {
//     *progress = 0.0;
//     PitchResult result;
//     result.period_s = audio_parameters.period_s;
//     result.f0_hz = this->f0_hz;
//     double duration_s = this->audio_reader->getStreams()[audio_stream_ind].duration_s;
//     this->audio_reader->initExtraction(audio_parameters, audio_stream_ind, timestart_s, timestop_s);
//     double dst_sample_rate_hz = (double)this->audio_reader->getOutputSampleRate();
//     double nan = std::numeric_limits<double>::quiet_NaN();
//     // Temporary variables
//     std::vector<double> buffer;
//     uint64_t k = 0;
//     int p[2];
//     int status;
//     bool finished = false;
//     while(!finished) {
//         if(pipe(p) < 0) {
//             exit(1);
//         }
//         for(int k_process=0; k_process<8; k_process++) {
//             finished = !(this->audio_reader->getNextBuffer(buffer));
//             if(finished) {
//                 break;
//             }
//             result.pitch_st.push_back(nan);
//             result.energy.push_back(nan);

//             pid_t pid = fork();
//             if(pid < 0) {
//                 // Failed to create child
//                 exit(1);
//             } else if(pid == 0) {
//                 // Child process
//                 close(p[PIPE_READ]);
//                 double energy = this->getEnergy(buffer);
//                 double pitch_hz = this->pitch_extractor->get_pitch(buffer, dst_sample_rate_hz);
//                 double pitch_st = convert_freq_to_tone(pitch_hz, this->f0_hz);
//                 pipe_element sample(k, energy, pitch_st);
//                 write(p[PIPE_WRITE], &sample, sizeof(pipe_element));
//                 close(p[PIPE_WRITE]);
//                 exit(0);
//             } else if (pid > 0) {
//                 k += 1;
//             }
//         }
//         while (wait(&status) > 0);
//         close(p[PIPE_WRITE]);
//         pipe_element sample;
//         while (read(p[PIPE_READ], &sample, sizeof(pipe_element)) > 0) {
//             result.energy[std::get<0>(sample)] = std::get<1>(sample);
//             result.pitch_st[std::get<0>(sample)] = std::get<2>(sample);
//         }
//         close(p[PIPE_READ]);
//         *progress = (audio_parameters.period_s * result.pitch_st.size() * 100.0) / duration_s;
//     }
//     return result;
// }

// #endif
