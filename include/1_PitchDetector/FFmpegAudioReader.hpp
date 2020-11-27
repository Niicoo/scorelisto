#ifndef FFMPEG_AUDIO_READER
#define FFMPEG_AUDIO_READER

#include <string>
extern "C" {
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
    #include <libavutil/audio_fifo.h>
}
#include "AudioReader.hpp"


void init_packet(AVPacket *packet);


class FFmpegAudioReader: public AudioReader {
public:
    // Constructors & Destructor
    FFmpegAudioReader(std::string filepath);
    ~FFmpegAudioReader();
    // Set parameters for buffer extraction 
    void initExtraction(const AudioReaderParameters & parameters=DEFAULT_AUDIO_READER_PARAMETERS,
                        unsigned int audio_stream_ind=0,
                        double timestart_s=-1.0,
                        double timestop_s=-1.0);
    // Get the next buffer of audio: initExtraction() must be called before calling this function
    bool getNextBuffer(std::vector<double> & buffer);
private:
    // Functions
    void FreeConvertedSamples();
    void FreeInputFrame();
    void FreeOutputFrame();
    void FreeCodecCtx();
    void FreeFormatCtx();
    void FreeSwrCtx();
    void FreeAudioFifo();
    void ConvertInputFrame();
    void InitConvertedSamples();
    void AddSamplesToFifo();
    void GetNbSamplesOut();
    void ConvertAndStoreOneFrame();
    void InitOutputFrame();
    void InitAudioFifo();
    void InitSwrCtx();
    void InitInputFrame();
    void DecodeOneAudioFrame();
    unsigned long int ExtractNumberOfSample();
    void GetCodecCtx(int indice_stream);
    std::vector<unsigned int> ExtractAudioStreamsIndex();
    void GetFormatCtxAndStreamInfo();
    void updateIndexNextIteration();
    // Private attributes
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVFrame *pInputFrame;
    bool data_present;
    bool finished;
    SwrContext *pSwrCtx;
    AVAudioFifo *pAudioFifo;
    AVFrame *pOutputFrame;
    uint8_t **ppConvertedInputSamples;
    int64_t nb_samples_out;
    int64_t iteration;
    int64_t temp_buffer_start;
    int64_t temp_buffer_stop;
    std::vector<double> temp_buffer;
    int64_t ind_start;
    int64_t ind_stop;
    int64_t WS_resampled;
};

#endif /* FFMPEG_AUDIO_READER */




