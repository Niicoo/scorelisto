#ifndef MC_LEOD_PITCH_EXTRACTOR_METHOD
#define MC_LEOD_PITCH_EXTRACTOR_METHOD

#include "PitchExtractorMethod.hpp"
#include <vector>


struct McLeodParameters {
    double cutoff;
    double small_cutoff;
    double lower_pitch_cutoff;
};
const McLeodParameters DEFAULT_MC_LEOD_PARAMETERS = {0.97, 0.5, 50.0};


class McLeodPitchExtractorMethod: public PitchExtractorMethod {
public:
    // Constructors & Destructor
    McLeodPitchExtractorMethod(const McLeodParameters & parameters=DEFAULT_MC_LEOD_PARAMETERS);
    McLeodPitchExtractorMethod(const McLeodPitchExtractorMethod & extractor);
    ~McLeodPitchExtractorMethod();
    // Get-Set Attributes
    void setCutoff(double cutoff);
    void setSmallCutoff(double small_cutoff);
    void setLowerPitch(double lower_pitch_cutoff);
    double getCutoff() const;
    double getSmallCutoff() const;
    double getLowerPitch() const;
    // Perform pitch detection on input buffer
    double get_pitch(const std::vector<double> & audio_buffer, double sample_rate_hz) const;
private:
    void normalizedSquareDifference(const std::vector<double> & audio_buffer, std::vector<double> & nsdf) const;
    void parabolicInterpolation(unsigned int tau, const std::vector<double> & nsdf, double & turningpoint_x, double & turningpoint_y) const;
    void peakPicking(const std::vector<double> & nsdf, std::vector<unsigned int> & max_positions) const;
    // Mc Leod Parameters
    double cutoff;
    double small_cutoff;
    double lower_pitch_cutoff;
};

#endif /* MC_LEOD_PITCH_EXTRACTOR_METHOD */
