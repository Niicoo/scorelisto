#include "StepDetector.hpp"
#include "HysteresisThreshold.hpp"
#include "CumulativeSumThreshold.hpp"
#include "common_tools.hpp"
#include <vector>
#include <cstdint>
#include <iostream>
#include <limits>
#include <cmath>
#include <numeric>


StepDetector::StepDetector(const PitchResult & pitch_result, const std::vector<bool> & mask/*={}*/) {
    // Pitch array
    if(pitch_result.pitch_st.empty()) {
        throw std::runtime_error("Pitch array cannot be empty");
    }
    this->pitch_st = pitch_result.pitch_st;
    this->resetPitchMask();
    // F0
    if(pitch_result.f0_hz <= 0) {
        throw std::runtime_error("f0 cannot be inferior or equal to 0");
    }
    this->f0_hz = pitch_result.f0_hz;
    // Period
    if(pitch_result.period_s <= 0) {
        throw std::runtime_error("Period of the pitch array cannot be inferior or equal to 0");
    }
    this->period_s = pitch_result.period_s;
    // Mask array
    if(mask.empty()){
        this->mask.assign(this->pitch_st.size(), false);
    } else {
        if(mask.size() != this->pitch_st.size()) {
            throw std::runtime_error("Mask array size is not the same as pitch array size");
        }
        this->mask = mask;
    }
    // Energy array
    if(!pitch_result.energy.empty()){
        if(pitch_result.energy.size() != this->pitch_st.size()) {
            throw std::runtime_error("Mask array size is not the same as pitch array size");
        }
        this->energy = pitch_result.energy;
        this->resetEnergyMask();
    }
}


StepDetector::~StepDetector() {
    // Destructor
}


std::vector<double> StepDetector::getPitch() const {
    return this->pitch_st;
}


std::vector<bool> StepDetector::getMask() const{
    return this->mask;
}


std::vector<bool> StepDetector::getPitchMask() const {
    return this->pitch_mask;
}


std::vector<bool> StepDetector::getEnergyMask() const {
    return this->energy_mask;
}


std::vector<double> StepDetector::getEnergy() const {
    return this->energy;
}


bool StepDetector::hasEnergy() const {
    return !this->energy.empty();
}


double StepDetector::getF0() const {
    return this->f0_hz;
}


double StepDetector::getPeriod() const {
    return this->period_s;
}


void StepDetector::setPitch(const std::vector<double> & pitch_st) {
    if(pitch_st.size() != this->pitch_st.size()) {
        throw std::runtime_error("Input vector sizes are not equal");
    }
    this->pitch_st = pitch_st;
    this->resetPitchMask();
}


void StepDetector::setMask(const std::vector<bool> & mask) {
    if(mask.size() != this->pitch_st.size()) {
        throw std::runtime_error("Input vector sizes are not equal");
    }
    this->mask = mask;
}


void StepDetector::setEnergyMask(const std::vector<bool> & mask) {
    if(!this->hasEnergy()) {
        throw std::runtime_error("Energy is not provided");
    }
    if(mask.size() != this->energy_mask.size()) {
        throw std::runtime_error("Input energy masks sizes are not equal");
    }
    this->energy_mask = mask;
}


void StepDetector::setPitchMask(const std::vector<bool> & mask) {
    if(mask.size() != this->pitch_mask.size()) {
        throw std::runtime_error("Input pitch masks sizes are not equal");
    }
    this->pitch_mask = mask;
}


void StepDetector::setEnergy(const std::vector<double> & energy) {
    if(energy.size() != this->pitch_st.size()) {
        throw std::runtime_error("Input vector sizes are not equal");
    }
    this->energy = energy;
    this->resetEnergyMask();
}


void StepDetector::setPitch(const std::vector<std::pair<size_t, double>> & indexes_and_values) {
    for(auto const& index_and_value: indexes_and_values) {
        this->pitch_st[index_and_value.first] = index_and_value.second;
    }
    this->resetPitchMask();
}


void StepDetector::setMask(const std::vector<std::pair<size_t, bool>> & indexes_and_values) {
    for(auto const& index_and_value: indexes_and_values) {
        this->mask[index_and_value.first] = index_and_value.second;
    }
}


void StepDetector::setEnergy(const std::vector<std::pair<size_t, double>> & indexes_and_values) {
    for(auto const& index_and_value: indexes_and_values) {
        this->energy[index_and_value.first] = index_and_value.second;
    }
    this->resetEnergyMask();
}


void StepDetector::resetPitchMask() {
    this->pitch_mask = std::vector<bool>(this->pitch_st.size(), false);
    for(size_t k = 0; k < this->pitch_st.size(); k++) {
        if((this->pitch_st[k] < 0) || std::isnan(this->pitch_st[k])) {
            this->pitch_mask[k] = true;
        }
    }
}


void StepDetector::resetEnergyMask() {
    this->energy_mask = std::vector<bool>(this->energy.size(), false);
    for(size_t k = 0; k < this->energy.size(); k++) {
        if((this->energy[k] < 0) || std::isnan(this->energy[k])) {
            this->energy_mask[k] = true;
        }
    }
}


std::vector<bool> StepDetector::getCombinedMasks() const {
    std::vector<bool> temp_mask = mask_logical_or(this->mask, this->pitch_mask);
    if(this->hasEnergy()) {
        temp_mask = mask_logical_or(temp_mask, this->energy_mask);
    }
    return temp_mask;
}


void StepDetector::maskThroughEnergyHysteresis(double threshold_off, double threshold_on) {
    if(!this->hasEnergy()) {
        throw std::runtime_error("Energy is not provided");
    }
    HysteresisThreshold thresholding(threshold_off, threshold_on, true);
    std::vector<bool> mask = thresholding.perform(this->energy);
    this->setEnergyMask(mask_logical_or(mask, this->energy_mask));
}


void StepDetector::maskThroughEnergyCumulativeSum(double threshold_min, double cumsum_min) {
    if(!this->hasEnergy()) {
        throw std::runtime_error("Energy is not provided");
    }
    CumulativeSumThreshold thresholding(threshold_min, cumsum_min, true);
    std::vector<bool> mask = thresholding.perform(this->energy);
    this->setEnergyMask(mask_logical_or(mask, this->energy_mask));
}


void StepDetector::maskThroughToneHeight(double tone_min, double tone_max) {
    std::vector<bool> mask(this->pitch_st.size(), false);
    for(size_t k = 0; k < this->pitch_st.size(); k++) {
        mask[k] = (this->pitch_st[k] < tone_min) || (this->pitch_st[k] > tone_max);
    }
    this->setPitchMask(mask_logical_or(mask, this->pitch_mask));
}


// These functions can recover NaN samples so they must be called first before any modification of the masks
// because the masks are resetted after this call
///////////////////////////////////////////////////////
void StepDetector::medianFilterPitch(double window_size_s) {
    if(window_size_s <= 0) {
        throw std::runtime_error("Windows size [s] for median filter must be superior than 0");
    }
    uint64_t kernel_size = (uint64_t)round(window_size_s / this->period_s);
    if((kernel_size % 2) == 0) {
        kernel_size += 1;
    }
    this->setPitch(median_filter(this->pitch_st, kernel_size));
}


void StepDetector::medianFilterEnergy(double window_size_s) {
    if(window_size_s <= 0) {
        throw std::runtime_error("Windows size [s] for median filter must be superior than 0");
    }
    uint64_t kernel_size = (uint64_t)round(window_size_s / this->period_s);
    if((kernel_size % 2) == 0) {
        kernel_size += 1;
    }
    this->setEnergy(median_filter(this->energy, kernel_size));
}
///////////////////////////////////////////////////////


void StepDetector::maskAutoEnergy() {
    if(!this->hasEnergy()) {
        throw std::runtime_error("Energy is not provided");
    }
    std::vector<double> energy_db;
    for(const auto & sample: this->energy) {
        energy_db.push_back(10.0 * log10(sample));
    }
    uint64_t bins = 1000;
    std::vector<double> points = get_histogram_points(energy_db, bins);

    double gaussiankdebandwidth = 0.1;
    GaussianKde density(energy_db, gaussiankdebandwidth);
    std::vector<double> histo = density.evaluate(points);
    std::vector<uint64_t> local_maxs = extract_local_min_max(histo, false, true);
    while(local_maxs.size() > 2) {
        gaussiankdebandwidth += 0.01;
        density = GaussianKde(energy_db, gaussiankdebandwidth);
        histo = density.evaluate(points);
        local_maxs = extract_local_min_max(histo, false, true);
    }
    if(local_maxs.size() != 2) {
        throw std::runtime_error("Auto masking energy failed");
    }
    uint64_t ind_max_noise = local_maxs[0];
    uint64_t ind_max_signal = local_maxs[1];
    auto it_min = std::min_element(histo.begin() + ind_max_noise, histo.begin() + ind_max_signal);
    auto ind_min = it_min - histo.begin();
    double threshold_db = points[ind_min] + (points[1] - points[0]) / 2.0;
    double threshold_nat_on = pow(10.0, threshold_db/10.0);
    double threshold_nat_off = pow(10.0, (threshold_db - 3.0)/10.0);
    this->maskThroughEnergyHysteresis(threshold_nat_off, threshold_nat_on);
}


void StepDetector::detectGroupsOfNotes(double min_group_size_s/*=50e-3*/) {
    if(min_group_size_s < 0) {
        throw std::runtime_error("Minimum length of note cannot be a negative number");
    }
    // Minimum number of samples
    uint64_t NbSamplesMin = (uint64_t)(round(min_group_size_s / this->period_s));
    // Reset the indexes of detected groups of notes
    this->groups.clear();
    std::pair<uint64_t, uint64_t> temp_indexes;
    bool group_detected = false;
    // Get the mask resulting for all combined masks
    std::vector<bool> total_mask = this->getCombinedMasks();
    // Initial conditions
    if(total_mask[0] == false) {
        group_detected = true;
        temp_indexes.first = 0;
    }
    for(size_t k = 1; k < total_mask.size(); k++) {
        if(!total_mask[k] && !group_detected) {
            group_detected = true;
            temp_indexes.first = k;
        }
        if(total_mask[k] && group_detected) {
            if((k - temp_indexes.first) >= NbSamplesMin) {
                temp_indexes.second = k;
                this->groups.push_back(temp_indexes);
            }
            group_detected = false;
        }
        // Condition for the end of the mask is a group has been detected
        if((k == (total_mask.size() -1)) && group_detected) {
            if((k + 1 - temp_indexes.first) >= NbSamplesMin) {
                temp_indexes.second = k + 1;
                this->groups.push_back(temp_indexes);
            }
        }
    }

}


double StepDetector::getMeanEnergy(size_t index_start, size_t index_stop) const {
    if(!this->hasEnergy()) {
        throw std::runtime_error("Energy is not provided");
    }
    double energy_sum = std::accumulate(this->energy.begin() + index_start, this->energy.begin() + index_stop, 0.0);
    return energy_sum / (double)(index_stop - index_start);
}

StepResult StepDetector::extractNotesFromGroups(const StepParameters & parameters) {
    if(this->groups.empty()) {
        throw std::runtime_error("No groups of note detected");
    }
    double nan = std::numeric_limits<double>::quiet_NaN();
    StepResult result;
    // Vector containing the temporary array of pitch of the group to process
    std::vector<double> temp_pitch;
    std::vector<std::pair<uint64_t, double>> temp_steps;
    AnalogNote temp_note;
    // Set the offset value
    result.offset_s = this->groups[0].first * this->period_s;
    // Iterating over all detected groups
    for(size_t ind_group = 0; ind_group < this->groups.size(); ind_group++) {
        // Adding the silence between each groups
        if(ind_group > 0) {
            temp_note.is_a_note = false;
            temp_note.length_s = (this->groups[ind_group].first - this->groups[ind_group - 1].second) * this->period_s;
            temp_note.pitch_st = nan;
            temp_note.energy = nan;
            temp_note.linked = false;
            result.notes.push_back(temp_note);
        }
        // Adding the notes of the group
        temp_pitch.assign(this->pitch_st.begin() + this->groups[ind_group].first, this->pitch_st.begin() + this->groups[ind_group].second);
        temp_steps = this->recoverSteps(temp_pitch, parameters);
        size_t k_start = this->groups[ind_group].first;
        size_t k_stop;
        for(size_t ind_step = 0; ind_step < temp_steps.size(); ind_step++) {
            k_stop = k_start + temp_steps[ind_step].first;
            temp_note.is_a_note = true;
            temp_note.length_s = temp_steps[ind_step].first * this->period_s;
            temp_note.pitch_st = temp_steps[ind_step].second;
            if(this->hasEnergy()) {
                temp_note.energy = this->getMeanEnergy(k_start, k_stop);
            } else {
                temp_note.energy = nan;
            }
            temp_note.linked = ind_step > 0;
            result.notes.push_back(temp_note);
            k_start = k_stop;
        }
    }
    return result;
}


StepResult StepDetector::perform(std::atomic<float> * progress, const StepParameters & parameters/*=DEFAULT_STEP_PARAMETERS*/) {
    *progress = 0.0;
    this->medianFilterPitch(parameters.median_filter_width_s);
    if(progress->load() < 0) {
        throw std::runtime_error("Process cancel by the user");
    }
    if(this->hasEnergy()) {
        this->medianFilterEnergy(parameters.median_filter_width_s);
        this->maskAutoEnergy();
    }
    if(progress->load() < 0) {
        throw std::runtime_error("Process cancel by the user");
    }
    *progress = 25.0;
    this->maskThroughToneHeight(parameters.min_pitch_st, parameters.max_pitch_st);
    if(progress->load() < 0) {
        throw std::runtime_error("Process cancel by the user");
    }
    *progress = 50.0;
    this->detectGroupsOfNotes(parameters.min_note_length_s);
    if(progress->load() < 0) {
        throw std::runtime_error("Process cancel by the user");
    }
    *progress = 75.0;
    StepResult result = this->extractNotesFromGroups(parameters);
    *progress = 100.0;
    return result;
}
