#ifndef HISTOGRAM_STEP_DETECTOR
#define HISTOGRAM_STEP_DETECTOR

#include "StepDetector.hpp"
#include "common_tools.hpp"


struct DetectedPitch {
    double length_s;
    double value_st;
};
bool compareByLength(const DetectedPitch &a, const DetectedPitch &b);
bool compareByPitch(const DetectedPitch &a, const DetectedPitch &b);


class HistogramStepDetector: public StepDetector {
public:
    // Constructors & Destructor
    HistogramStepDetector(const PitchResult & pitch_result, const std::vector<bool> & mask={}, double gaussian_mid_height_width_s=0.5);
    ~HistogramStepDetector();
    // Method to override performing the steps recovering
    std::vector<std::pair<uint64_t, double>> recoverSteps(const std::vector<double> & pitch_buffer, const StepParameters & parameters=DEFAULT_STEP_PARAMETERS);
private:
    Histogram getHistogram(const std::vector<double> & pitch_buffer) const;
    void smoothHistogram(Histogram & histo) const;
    void detectPitchs(const std::vector<uint64_t> & indexes, const Histogram & histo);
    void deleteNotesTooShort(double min_note_length_s);
    void deleteNotesTooClose(double min_note_gap_st);
    size_t getIndexBestFit(double pitch_st) const;
    std::vector<size_t> calculateBestFit(const std::vector<double> & signal);
    void clearFittedSignal(std::vector<size_t> & fitted_signal, double min_note_length_s);
    std::vector<std::pair<uint64_t, double>> formatOutput(const std::vector<size_t> & ind_pitchs) const;
    double gaussian_mid_height_width_s;
    double histogram_step_st;
    double std_gaussian;
    std::vector<double> gaussian_shape;
    std::vector<DetectedPitch> pitchs;
};

#endif /* HISTOGRAM_STEP_DETECTOR */
