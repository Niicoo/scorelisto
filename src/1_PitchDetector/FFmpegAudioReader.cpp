#include "FFmpegAudioReader.hpp"
#include <iostream>
#include <string>
#include <fstream>

extern "C" {
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
    #include <libavutil/audio_fifo.h>
    #include <libavutil/opt.h>
}


void init_packet(AVPacket *packet) {
    av_init_packet(packet);
    /* Set the packet data and size so that it is recognized as being empty. */
    packet->data = NULL;
    packet->size = 0;
}


/* PRIVATE FUNCTIONS */
/*********************/
void FFmpegAudioReader::FreeConvertedSamples() {
    if (this->ppConvertedInputSamples) {
        av_freep(&(this->ppConvertedInputSamples)[0]);
        free(this->ppConvertedInputSamples);
    }
}


void FFmpegAudioReader::FreeInputFrame() {
    av_frame_free(&(this->pInputFrame));
    this->pInputFrame = NULL;
}


void FFmpegAudioReader::FreeOutputFrame() {
    av_frame_free(&(this->pOutputFrame));
    this->pOutputFrame = NULL;
}


void FFmpegAudioReader::FreeCodecCtx() {
    avcodec_free_context(&(this->pCodecCtx));
    this->pCodecCtx = NULL;
}


void FFmpegAudioReader::FreeFormatCtx() {
    avformat_close_input(&(this->pFormatCtx));
    this->pFormatCtx = NULL;
}


void FFmpegAudioReader::FreeSwrCtx() {
    swr_free(&(this->pSwrCtx));
    this->pSwrCtx = NULL;
}


void FFmpegAudioReader::FreeAudioFifo() {
    av_audio_fifo_free(this->pAudioFifo);
    this->pAudioFifo = NULL;
}


void FFmpegAudioReader::ConvertInputFrame() {
    /* Convert the samples using the resampler. */
    if (swr_convert(this->pSwrCtx,
                    this->ppConvertedInputSamples,
                    this->nb_samples_out,
                    (const uint8_t**)this->pInputFrame->extended_data,
                    this->pInputFrame->nb_samples) < 0) {
        throw std::runtime_error("Could not convert input samples");
    }
}


void FFmpegAudioReader::InitConvertedSamples() {
    int error;
    /* Allocate as many pointers as there are audio channels.
     * Each pointer will later point to the audio samples of the corresponding
     * channels (although it may be NULL for interleaved formats).
     */ 
    int64_t out_ch_layout;
    enum AVSampleFormat out_sample_fmt;
    av_opt_get_int(this->pSwrCtx, "ocl", 0, &out_ch_layout);
    av_opt_get_sample_fmt(this->pSwrCtx, "osf", 0, &out_sample_fmt);

    int nb_channels = av_get_channel_layout_nb_channels(out_ch_layout);
    if (!(this->ppConvertedInputSamples = (uint8_t**)calloc(nb_channels, sizeof(**(this->ppConvertedInputSamples))))) {
        throw std::runtime_error("Could not allocate converted input sample pointers");
    }
    /* Allocate memory for the samples of all channels in one consecutive
     * block for convenience. */
    if ((error = av_samples_alloc(this->ppConvertedInputSamples, NULL,
                                  nb_channels,
                                  this->nb_samples_out,
                                  out_sample_fmt, 0)) < 0) {
        this->FreeConvertedSamples();
        throw std::runtime_error("Could not allocate converted input samples");
    }
}


void FFmpegAudioReader::AddSamplesToFifo() {
    /* Make the FIFO as large as it needs to be to hold both,
     * the old and the new samples. */
    if (av_audio_fifo_realloc(this->pAudioFifo, av_audio_fifo_size(this->pAudioFifo) + this->nb_samples_out) < 0) {
        throw std::runtime_error("Could not reallocate FIFO");
    }
    /* Store the new samples in the FIFO buffer. */
    if (av_audio_fifo_write(this->pAudioFifo, (void **)(this->ppConvertedInputSamples),
                            this->nb_samples_out) < this->nb_samples_out) {
        throw std::runtime_error("Could not write data to FIFO");
    }
}


void FFmpegAudioReader::GetNbSamplesOut() {
    int64_t sample_rate_out;
    int64_t sample_rate_in;
    av_opt_get_int(this->pSwrCtx, "osr", 0, &sample_rate_out);
    av_opt_get_int(this->pSwrCtx, "isr", 0, &sample_rate_in);
    this->nb_samples_out = (int64_t)floor(((double)(this->pInputFrame->nb_samples-1)/(double)sample_rate_in)*(double)sample_rate_out) + 1;
}


void FFmpegAudioReader::ConvertAndStoreOneFrame() {
    /* Temporary storage for the converted input samples. */
    const std::runtime_error *error = NULL;
    try {
        /* Initialize the temporary storage for the converted input samples. */
        this->GetNbSamplesOut();

        this->InitConvertedSamples();
        /* Convert the input samples to the desired output sample format.
         * This requires a temporary storage provided by this->ppConvertedInputSamples. */
        this->ConvertInputFrame();
        /* Add the converted input samples to the FIFO buffer for later processing. */
        this->AddSamplesToFifo();
    } catch(const std::runtime_error& e) {
        error = &e;
    }
    // Free memory
    this->FreeConvertedSamples();
    // If error != NULL then an error occured
    if(error != NULL) {
        throw *error;
    }
}


void FFmpegAudioReader::InitOutputFrame() {
    int64_t dst_ch_layout;
    enum AVSampleFormat dst_sample_fmt;
    int64_t dst_rate;
    av_opt_get_int(this->pSwrCtx, "ocl", 0, &dst_ch_layout);
    av_opt_get_int(this->pSwrCtx, "osr", 0, &dst_rate);
    av_opt_get_sample_fmt(this->pSwrCtx, "osf", 0, &dst_sample_fmt);
    /* Create a new frame to store the audio samples. */
    if (!(this->pOutputFrame = av_frame_alloc())) {
        throw std::runtime_error("Could not allocate output frame");
    }
    /* Set the frame's parameters, especially its size and format.
     * av_frame_get_buffer needs this to allocate memory for the
     * audio samples of the frame.
     * Default channel layouts based on the number of channels
     * are assumed for simplicity. */
    this->pOutputFrame->nb_samples     = this->WS_resampled;
    this->pOutputFrame->channel_layout = dst_ch_layout;
    this->pOutputFrame->format         = dst_sample_fmt;
    this->pOutputFrame->sample_rate    = dst_rate;
    /* Allocate the samples of the created frame. This call will make
     * sure that the audio frame can hold as many samples as specified. */
    if (av_frame_get_buffer(this->pOutputFrame, 0) < 0) {
        this->FreeOutputFrame();
        throw std::runtime_error("Could not allocate output frame samples");
    }
}


void FFmpegAudioReader::InitAudioFifo() {
    int64_t dst_ch_layout;
    enum AVSampleFormat dst_sample_fmt;
    av_opt_get_int(this->pSwrCtx, "ocl", 0, &dst_ch_layout);
    av_opt_get_sample_fmt(this->pSwrCtx, "osf", 0, &dst_sample_fmt);
    int nb_channels = av_get_channel_layout_nb_channels(dst_ch_layout); 
    /* Create the FIFO buffer based on the specified output sample format. */
    if (!(this->pAudioFifo = av_audio_fifo_alloc(dst_sample_fmt, nb_channels, 1))) {
        throw std::runtime_error("Could not allocate FIFO");
    }
}


void FFmpegAudioReader::InitSwrCtx() {
    this->pSwrCtx = swr_alloc_set_opts(NULL,
                                        AV_CH_LAYOUT_MONO,
                                        AV_SAMPLE_FMT_DBL,
                                        this->dst_rate_hz,
                                        av_get_default_channel_layout(this->pCodecCtx->channels),
                                        this->pCodecCtx->sample_fmt,
                                        this->pCodecCtx->sample_rate,
                                        0, NULL);
    if (!this->pSwrCtx) {
        throw std::runtime_error("Could not allocate resample context");
    }
    /* Open the resampler with the specified parameters. */
    if (swr_init(this->pSwrCtx) < 0) {
        this->FreeSwrCtx();
        throw std::runtime_error("Could not open resample context");
    }
}


void FFmpegAudioReader::InitInputFrame() {
    if (!(this->pInputFrame = av_frame_alloc())) {
        throw std::runtime_error("Could not allocate input frame");
    }
}


void FFmpegAudioReader::DecodeOneAudioFrame() {
    this->data_present = false;
    /* Packet used for temporary storage. */
    AVPacket input_packet;
    int error;
    init_packet(&input_packet);
    /* Read one audio frame from the input file into a temporary packet. */
    if ((error = av_read_frame(this->pFormatCtx, &input_packet)) < 0) {
        /* If we are at the end of the file, flush the decoder below. */
        if (error == AVERROR_EOF)
            this->finished = true;
        else {
            av_packet_unref(&input_packet);
            throw std::runtime_error("Could not read frame");
        }
    }
    /* Send the audio frame stored in the temporary packet to the decoder.
     * The input audio stream decoder is used to do this. */
    if ((error = avcodec_send_packet(this->pCodecCtx, &input_packet)) < 0) {
        av_packet_unref(&input_packet);
        throw std::runtime_error("Could not send packet for decoding");
    }
    /* Receive one frame from the decoder. */
    error = avcodec_receive_frame(this->pCodecCtx, this->pInputFrame);
    av_packet_unref(&input_packet);
    /* If the decoder asks for more data to be able to decode a frame,
     * return indicating that no data is present. */
    if (error == AVERROR(EAGAIN)) {
        this->data_present = false;
    /* If the end of the input file is reached, stop decoding. */
    } else if (error == AVERROR_EOF) {
        this->finished = true;
    } else if (error < 0) {
        throw std::runtime_error("Could not decode frame");
    /* Default case: Return decoded data. */
    } else {
        this->data_present = true;
    }
}


unsigned long int FFmpegAudioReader::ExtractNumberOfSample() {
    unsigned long int nb_samples = 0;
    /* Temporary storage of the input samples of the frame read from the file. */
    this->data_present = false;
    this->finished = false;
    /* Initialize temporary storage for one input frame. */
    this->InitInputFrame();
    /* Decode frames of audio samples. */
    while(1) {
        this->DecodeOneAudioFrame();
        if(this->data_present) {
            nb_samples = nb_samples + this->pInputFrame->nb_samples;
        }
        if(this->finished) {
            break;
        }
    }
    // Free frame 
    this->FreeInputFrame();
    return nb_samples;
}


void FFmpegAudioReader::GetCodecCtx(int ind_stream) {
    AVCodec *input_codec;
    /* Find a decoder for the audio stream. */
    if (!(input_codec = avcodec_find_decoder(this->pFormatCtx->streams[ind_stream]->codecpar->codec_id))) {
        throw std::runtime_error("Could not find input codec");
    }
    /* Allocate a new decoding context. */
    this->pCodecCtx = avcodec_alloc_context3(input_codec);
    if (!this->pCodecCtx) {
        throw std::runtime_error("Could not allocate a decoding context");
    }
    /* Initialize the stream parameters with demuxer information. */
    if (avcodec_parameters_to_context(this->pCodecCtx, this->pFormatCtx->streams[ind_stream]->codecpar) < 0) {
        this->FreeCodecCtx();
        throw std::runtime_error("Could not initialize the stream parameters");
    }
    /* Open the decoder for the audio stream to use it later. */
    if (avcodec_open2(this->pCodecCtx, input_codec, NULL) < 0) {
        this->FreeCodecCtx();
        throw std::runtime_error("Could not open input codec");
    }
}


std::vector<unsigned int> FFmpegAudioReader::ExtractAudioStreamsIndex() {
    std::vector<unsigned int> indices;
    for(unsigned int k=0; k < this->pFormatCtx->nb_streams; k++) {
        if(this->pFormatCtx->streams[k]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO) {
            indices.push_back(k);
        }
    }
    return indices;
}


void FFmpegAudioReader::GetFormatCtxAndStreamInfo() {
    const char * tempfilepath = this->filepath.c_str();
    /* Open the input file to read from it. */
    if (avformat_open_input(&(this->pFormatCtx), tempfilepath, NULL, NULL) < 0) {
        this->pFormatCtx = NULL;
        throw std::runtime_error("Could not open input file");
    }
    /* Get information on the input file (number of streams etc.). */
    if (avformat_find_stream_info(this->pFormatCtx, NULL) < 0) {
        this->FreeFormatCtx();
        throw std::runtime_error("Could not find stream info");
    }
}


void FFmpegAudioReader::updateIndexNextIteration(){
    this->iteration += 1;
    this->ind_start = (int64_t)round((this->dst_period_s * this->iteration)*(double)this->dst_rate_hz);
    this->ind_stop = this->ind_start + this->WS_resampled;
}


/* PUBLIC FUNCTIONS */
/********************/
FFmpegAudioReader::FFmpegAudioReader(std::string filepath) : AudioReader(filepath) {
    this->pFormatCtx = NULL;
    this->pCodecCtx = NULL;
    this->pInputFrame = NULL;
    this->pSwrCtx = NULL;
    this->pAudioFifo = NULL;
    this->pOutputFrame = NULL;
    this->ppConvertedInputSamples = NULL;
    this->nb_samples_out = 0;
    this->data_present = false;
    this->finished = false;
    unsigned long int nb_samples;
    // Get format context (this->pFormatCtx)
    this->GetFormatCtxAndStreamInfo();
    // Get indices of audio streams only
    std::vector<unsigned int> indices_audio_streams;
    indices_audio_streams = this->ExtractAudioStreamsIndex();
    for(auto& ind : indices_audio_streams) {
        // Get decoder and Get number of samples
        try {
            // Get codec context (this->pCodecCtx)
            this->GetCodecCtx(ind);
            nb_samples = this->ExtractNumberOfSample();
        }
        catch(std::runtime_error& e) {
            // Failed to get decoder
            this->FreeCodecCtx();
            throw e;
            continue;
        }
        // Add this stream as valid audio stream
        AudioStream temp_stream;
        temp_stream.ind_stream = (unsigned int)ind;
        temp_stream.format = pCodecCtx->codec->name;
        temp_stream.sample_rate_hz = pFormatCtx->streams[ind]->codecpar->sample_rate;
        temp_stream.nb_channels = (unsigned int)pFormatCtx->streams[ind]->codecpar->channels;
        temp_stream.bit_rate = (unsigned long int)pCodecCtx->bit_rate;
        temp_stream.duration_s = (double)pFormatCtx->streams[ind]->duration / (double)pFormatCtx->streams[ind]->time_base.den;
        temp_stream.nb_samples = nb_samples;
        this->streams.push_back(temp_stream);
        this->FreeCodecCtx();
    }
    // Free the codec context and everything associated with it and write NULL to the provided pointer.
    this->FreeFormatCtx();
    // Check if a valid audio stream has been found
    if(this->streams.size() == 0) {
        throw std::runtime_error("Could not find valid audio stream");
    }

}

void FFmpegAudioReader::initExtraction( const AudioReaderParameters & parameters/*=DEFAULT_AUDIO_READER_PARAMETERS*/,
                                        unsigned int audio_stream_ind/*=0*/,
                                        double timestart_s/*=-1.0*/,
                                        double timestop_s/*=-1.0*/
                                        ) {
    this->FreeInputFrame();
    this->FreeAudioFifo();
    this->FreeSwrCtx();
    this->FreeOutputFrame();
    this->FreeCodecCtx();
    this->FreeFormatCtx();
    // Time start and time stop
    if(timestart_s >= 0) {
        throw std::runtime_error("Function not yet implemented");
    }
    if(timestop_s >= 0) {
        throw std::runtime_error("Function not yet implemented");
    }
    if(audio_stream_ind >= this->streams.size()) {
        throw std::runtime_error("Invalid input index");
    }
    if(parameters.resample_rate_hz < 0) {
        // Same sample rate as input
        this->dst_rate_hz = this->streams[audio_stream_ind].sample_rate_hz;
    } else {
        this->dst_rate_hz = parameters.resample_rate_hz;
    }
    this->dst_period_s = parameters.period_s;
    this->dst_windowsize_s = parameters.windowstimesize_s;
    // Get format context
    this->GetFormatCtxAndStreamInfo();
    // Get codec context
    int ind = this->streams[audio_stream_ind].ind_stream;
    this->GetCodecCtx(ind);
    /* Initialize the resampler to be able to convert audio sample formats. */
    this->InitSwrCtx();
    /* Initialize the FIFO buffer to store audio samples to be encoded. */
    this->InitAudioFifo();
    // Size of the windows 
    this->WS_resampled = (int64_t)round(this->dst_windowsize_s*(double)this->dst_rate_hz);
    this->InitOutputFrame();
    this->InitInputFrame();
    this->finished = false;
    // Index of the first audio buffer to get
    this->iteration = -1;
    this->temp_buffer_start = 0;
    this->temp_buffer_stop = 0;
    this->temp_buffer.clear();
}


bool FFmpegAudioReader::getNextBuffer(std::vector<double> & buffer) {
    buffer.clear();
    this->updateIndexNextIteration();
    const std::runtime_error *error = NULL;
    double* dbl_ptr = NULL;
    try {
        while((this->temp_buffer_stop < this->ind_stop) && !this->finished) {
            // Decode one frame
            while((av_audio_fifo_size(this->pAudioFifo) < this->WS_resampled) && !this->finished) {
                this->DecodeOneAudioFrame();
                if (this->data_present) {
                    this->ConvertAndStoreOneFrame();
                }
            }
            // Fill Output frame as much as possible
            while (av_audio_fifo_size(this->pAudioFifo) >= this->WS_resampled) {
                if (av_audio_fifo_read(this->pAudioFifo, (void **)this->pOutputFrame->data, this->WS_resampled) < this->WS_resampled) {
                    this->FreeOutputFrame();
                    throw std::runtime_error("Could not read data from FIFO");
                }
                // Update audio buffer vector and update stop index
                dbl_ptr = (double*)*(this->pOutputFrame->data);
                // std::vector<double> tempbuf(dbl_ptr, dbl_ptr + this->WS_resampled);
                this->temp_buffer.insert( this->temp_buffer.end(), dbl_ptr, dbl_ptr + this->WS_resampled);
                this->temp_buffer_stop += this->WS_resampled;
            }
        }
        if(this->temp_buffer_stop >= this->ind_stop) {
            // Translate the vector to the first element
            auto first = this->temp_buffer.begin() + (this->ind_start - this->temp_buffer_start);
            this->temp_buffer.erase(this->temp_buffer.begin(), first);
            this->temp_buffer_start = this->ind_start;
            // Extract the part of the buffer to sent
            buffer.assign(this->temp_buffer.begin(), this->temp_buffer.begin() + this->WS_resampled);
            return true;
        }/* else {
            // Finished
            return false;
        }*/
    } catch(const std::runtime_error& e) {
        error = &e;
    }
    // Free memory
    this->FreeInputFrame();
    this->FreeAudioFifo();
    this->FreeSwrCtx();
    this->FreeOutputFrame();
    this->FreeCodecCtx();
    this->FreeFormatCtx();
    // If error != NULL then an error occured
    if(error != NULL) {
        throw *error;
    }
    return false;
}


FFmpegAudioReader::~FFmpegAudioReader() {
    // Destructor
}
