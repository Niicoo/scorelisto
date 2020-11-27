#include <algorithm>
#include <cmath>
#include <climits>
#include <iostream>
#include "HistogramStepDetector.hpp"
#include "common_tools.hpp"


bool compareByLength(const DetectedPitch &a, const DetectedPitch &b) {
    return a.length_s < b.length_s;
}


bool compareByPitch(const DetectedPitch &a, const DetectedPitch &b) {
    return a.value_st < b.value_st;
}


HistogramStepDetector::HistogramStepDetector(const PitchResult & pitch_result, const std::vector<bool> & mask/*={}*/, double gaussian_mid_height_width_s/*=0.5*/) : StepDetector(pitch_result, mask) {
    // Constructor
    this->gaussian_mid_height_width_s = gaussian_mid_height_width_s;
    this->histogram_step_st = 0.01;
    this->std_gaussian = (this->gaussian_mid_height_width_s/this->histogram_step_st)/2.355;
    uint64_t nb_samples_gaussian = (uint64_t)(ceil(this->std_gaussian * 3.0 * 2.0));
    // Ensuring that the number of point is odd ( to get a maximum value at the middle)
    if((nb_samples_gaussian % 2) == 0) {
        nb_samples_gaussian += 1;
    }
    this->gaussian_shape = gaussian(nb_samples_gaussian, this->std_gaussian);
}


HistogramStepDetector::~HistogramStepDetector() {
    // Destructor
}


Histogram HistogramStepDetector::getHistogram(const std::vector<double> & pitch_buffer) const {
    if(pitch_buffer.empty()) {
        throw std::runtime_error("Input vector of pitchs is empty");
    }
    double min_value = *std::min_element(pitch_buffer.begin(), pitch_buffer.end());
    double max_value = *std::max_element(pitch_buffer.begin(), pitch_buffer.end());
    uint64_t nb_step_min = (uint64_t)floor(min_value / this->histogram_step_st);
    uint64_t nb_step_max = (uint64_t)ceil(max_value / this->histogram_step_st);
    if(nb_step_min == nb_step_max) {
        nb_step_max += 1;
        nb_step_min -= 1;
    }
    double min_st = nb_step_min * this->histogram_step_st;
    double max_st = nb_step_max * this->histogram_step_st;
    Histogram result = histogram(pitch_buffer, nb_step_max - nb_step_min, min_st, max_st);
    return result;
}


void HistogramStepDetector::smoothHistogram(Histogram & histo) const {
    if(histo.data.empty()) {
        throw std::runtime_error("Input signal is empty");
    }
    std::vector<double> histo_convolved = convolve(histo.data, this->gaussian_shape);
    histo.data.assign(histo_convolved.begin(), histo_convolved.end());
}


void HistogramStepDetector::detectPitchs(const std::vector<uint64_t> & indexes, const Histogram & histo) {
    if(indexes.empty()) {
        throw std::runtime_error("Input vector of indexes is empty");
    }
    this->pitchs.clear();
    DetectedPitch temp_pitch;
    for(auto & ind: indexes) {
        temp_pitch.length_s = histo.data[ind] * this->period_s;
        temp_pitch.value_st = histo.x[ind] + this->histogram_step_st/2.0;
        this->pitchs.push_back(temp_pitch);
    }
}


void HistogramStepDetector::deleteNotesTooShort(double min_note_length_s) {
    std::sort(this->pitchs.begin(), this->pitchs.end(), compareByLength);
    while(this->pitchs.size() > 1) {
        if(this->pitchs[0].length_s < min_note_length_s) {
            this->pitchs.erase(this->pitchs.begin());
        } else {
            break;
        }
    }
}


void HistogramStepDetector::deleteNotesTooClose(double min_note_gap_st) {
    std::sort(this->pitchs.begin(), this->pitchs.end(), compareByPitch);
    double temp_gap;
    double min_gap;
    size_t ind_min_gap;
    bool note_deleted = true;
    while(note_deleted) {
        note_deleted = false;
        min_gap = std::numeric_limits<double>::max();
        for(size_t ind = 1; ind < this->pitchs.size(); ind++) {
            temp_gap = this->pitchs[ind].value_st - this->pitchs[ind - 1].value_st;
            if(temp_gap < min_gap) {
                min_gap = temp_gap;
                ind_min_gap = ind;
            }
        }
        if(min_gap < min_note_gap_st) {
            if(this->pitchs[ind_min_gap].length_s > this->pitchs[ind_min_gap - 1].length_s) {
                this->pitchs.erase(this->pitchs.begin() + ind_min_gap - 1);
            } else {
                this->pitchs.erase(this->pitchs.begin() + ind_min_gap);
            }
            note_deleted = true;
        }
    }
}


size_t HistogramStepDetector::getIndexBestFit(double pitch_st) const {
    size_t result = 0;
    double min_abs_st = std::numeric_limits<double>::max();
    double temp_abs_st;
    for(size_t ind = 0; ind < this->pitchs.size(); ind++) {
        temp_abs_st = abs(this->pitchs[ind].value_st - pitch_st);
        if(temp_abs_st < min_abs_st) {
            result = ind;
            min_abs_st = temp_abs_st;
        }
    }
    return result;
}


std::vector<size_t> HistogramStepDetector::calculateBestFit(const std::vector<double> & signal) {
    std::sort(this->pitchs.begin(), this->pitchs.end(), compareByPitch);
    std::vector<size_t> result;
    size_t ind_best_fit_temp;
    for(const auto & sample_st: signal) {
        ind_best_fit_temp = this->getIndexBestFit(sample_st);
        result.push_back(ind_best_fit_temp);
    }
    return result;
}


void HistogramStepDetector::clearFittedSignal(std::vector<size_t> & fitted_signal, double min_note_length_s) {
    int64_t WindowsSizeDiv2 = (uint64_t)round(min_note_length_s / this->period_s);
    int64_t N = fitted_signal.size();
    if(N <= (2 * WindowsSizeDiv2 + 1)) {
        size_t ind_pitch = most_commun_value<size_t>(fitted_signal);
        std::fill(fitted_signal.begin(), fitted_signal.end(), ind_pitch);
    } else {
        bool modification = true;
        std::vector<size_t> new_signal(fitted_signal);
        std::vector<size_t> original_signal;
        std::vector<size_t> temp_signal;
        size_t mcv;
        while(modification) {
            modification = false;
            // original_signal = new_signal;
            original_signal.assign(new_signal.begin(), new_signal.end());
            // Beginning of the array
            temp_signal.assign(original_signal.begin(), original_signal.begin() + 2 * WindowsSizeDiv2 + 1);
            mcv = most_commun_value<size_t>(temp_signal);
            for(int64_t ind = 0; ind < WindowsSizeDiv2; ind ++) {
                if(new_signal[ind] != mcv) {
                    new_signal[ind] = mcv;
                    modification = true;
                    
                }
            }
            // Middle of the array
            for(int64_t ind = WindowsSizeDiv2; ind < (N - WindowsSizeDiv2 - 1); ind ++) {
                temp_signal.assign(original_signal.begin() + ind - WindowsSizeDiv2, original_signal.begin() + ind + WindowsSizeDiv2 + 1);
                mcv = most_commun_value<size_t>(temp_signal);
                if(new_signal[ind] != mcv) {
                    new_signal[ind] = mcv;
                    modification = true;
                }
            }
            // End of the array
            temp_signal.assign(original_signal.end() - 2 * WindowsSizeDiv2 - 1, original_signal.end());
            mcv = most_commun_value<size_t>(temp_signal);
            for(int64_t ind = (N - WindowsSizeDiv2 - 1) ; ind < N; ind ++) {
                if(new_signal[ind] != mcv) {
                    new_signal[ind] = mcv;
                    modification = true;
                }
            }
        }
        // fitted_signal = original_signal;
        fitted_signal.assign(original_signal.begin(), original_signal.end());
    }
}


std::vector<std::pair<uint64_t, double>> HistogramStepDetector::formatOutput(const std::vector<size_t> & ind_pitchs) const {
    if(ind_pitchs.empty()) {
        throw std::runtime_error("Indexes of pitchs cannot be an empty vector");
    }
    std::vector<std::pair<uint64_t, double>> result;
    std::pair<uint64_t, double> temp;
    uint64_t count = 0;
    double current_value = 0.0;
    for(size_t ind = 0; ind < ind_pitchs.size(); ind++) {
        if(ind == 0) {
            count = 1;
            current_value = this->pitchs[ind_pitchs[ind]].value_st;
        } else if(ind_pitchs[ind] == ind_pitchs[ind-1]) {
            count += 1;
        } else {
            temp.first = (uint64_t)count;
            temp.second = current_value;
            result.push_back(temp);
            count = 1;
            current_value = this->pitchs[ind_pitchs[ind]].value_st;
        }
    }
    temp.first = count;
    temp.second = current_value;
    result.push_back(temp);
    return result;
}


std::vector<std::pair<uint64_t, double>> HistogramStepDetector::recoverSteps(const std::vector<double> & pitch_buffer, const StepParameters & parameters/*=DEFAULT_STEP_PARAMETERS*/) {
    // Get the classic histogram
    Histogram histo = this->getHistogram(pitch_buffer);
    // Smoothing the histogram using gaussian function
    this->smoothHistogram(histo);
    // Extract all the local maximum indexes
    std::vector<uint64_t> max_indexes = extract_local_min_max(histo.data, true, true);
    // Get the length and pitch of all the indexes
    this->detectPitchs(max_indexes, histo);
    // Delete too short notes 
    this->deleteNotesTooShort(parameters.min_note_length_s);
    // Delete notes too closed from each other (the lowest is deleted)
    this->deleteNotesTooClose(parameters.min_note_gap_st);
    // Calculate the signal that fit the best the allowed pitch levels of notes
    std::vector<size_t> signal_fitted = this->calculateBestFit(pitch_buffer);
    // Clear the fitted signal of too short notes
    this->clearFittedSignal(signal_fitted, parameters.min_note_length_s);
    // Generate to the output format
    std::vector<std::pair<uint64_t, double>> result;
    result = this->formatOutput(signal_fitted);
    return result;
}