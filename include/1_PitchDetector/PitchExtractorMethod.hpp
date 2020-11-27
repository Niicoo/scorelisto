#ifndef PITCH_EXTRACTOR_METHOD
#define PITCH_EXTRACTOR_METHOD

#include <string>
#include <vector>

class PitchExtractorMethod {
public:
    // Constructors & Destructor
    PitchExtractorMethod();
    virtual ~PitchExtractorMethod();
    // Get pitch from the input buffer
    virtual double get_pitch(const std::vector<double> & audio_buffer, double sample_rate_hz) const = 0;
};

#endif /* PITCH_EXTRACTOR_METHOD */
