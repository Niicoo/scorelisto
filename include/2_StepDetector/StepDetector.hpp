#ifndef STEP_DETECTOR
#define STEP_DETECTOR

#include <vector>
#include <cstdint>
#include <atomic>
#include "PitchDetector.hpp"
#include "common_tools.hpp"

struct AnalogNote {
    bool is_a_note;
    double length_s;
    double pitch_st;
    double energy;
    bool linked;
};


struct StepResult {
    std::vector<AnalogNote> notes;
    double offset_s;
};


struct StepParameters {
    double min_note_length_s;
    double min_note_gap_st;
    double median_filter_width_s;
    double min_pitch_st;
    double max_pitch_st;
};
const StepParameters DEFAULT_STEP_PARAMETERS = {100e-3, 2/3.0, 20e-3, 0.0, 200.0};


class StepDetector {
public:
    // Constructors & Destructor
    StepDetector(const PitchResult & pitch_result, const std::vector<bool> & mask={});
    virtual ~StepDetector();
    // Perform step detection
    StepResult perform(std::atomic<float> * progress, const StepParameters & parameters=DEFAULT_STEP_PARAMETERS);
    // Get and Set methods
    std::vector<double> getPitch() const;
    std::vector<bool> getMask() const;
    std::vector<bool> getPitchMask() const;
    std::vector<bool> getEnergyMask() const;
    std::vector<double> getEnergy() const;
    double getF0() const;
    double getPeriod() const;
    // Get-Set Attributes
    void setPitch(const std::vector<double> & pitch_st);
    void setMask(const std::vector<bool> & mask);
    void setEnergyMask(const std::vector<bool> & mask);
    void setPitchMask(const std::vector<bool> & mask);
    void setEnergy(const std::vector<double> & energy);
    void setPitch(const std::vector<std::pair<size_t, double>> & indexes_and_values);
    void setMask(const std::vector<std::pair<size_t, bool>> & indexes_and_values);
    void setEnergy(const std::vector<std::pair<size_t, double>> & indexes_and_values);
    void resetPitchMask();
    void resetEnergyMask();
    bool hasEnergy() const;
    std::vector<bool> getCombinedMasks() const;
    double getMeanEnergy(size_t index_start, size_t index_stop) const;
    void maskThroughEnergyHysteresis(double threshold_off, double threshold_on);
    void maskThroughEnergyCumulativeSum(double threshold_min, double cumsum_min);
    void maskThroughToneHeight(double tone_min, double tone_max);
    void medianFilterPitch(double window_size_s);
    void medianFilterEnergy(double window_size_s);
    void maskAutoEnergy();
    StepResult extractNotesFromGroups(const StepParameters & parameters);
    virtual std::vector<std::pair<uint64_t, double>> recoverSteps(const std::vector<double> & pitch_buffer, const StepParameters & parameters) = 0;
protected:
    double period_s;
private:
    // private Methods
    void detectGroupsOfNotes(double min_group_size_s=50e-3);
    // Private attributes
    std::vector<double> pitch_st;
    double f0_hz;
    bool has_energy;
    std::vector<bool> mask;
    std::vector<bool> pitch_mask;
    std::vector<bool> energy_mask;
    std::vector<double> energy;
    std::vector<std::pair<uint64_t, uint64_t>> groups;
};

#endif /* STEP_DETECTOR */
