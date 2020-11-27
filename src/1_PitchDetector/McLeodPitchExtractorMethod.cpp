#include "McLeodPitchExtractorMethod.hpp"
#include <limits>
#include <numeric>
#include <algorithm>

McLeodPitchExtractorMethod::McLeodPitchExtractorMethod(const McLeodParameters & parameters/*=DEFAULT_MC_LEOD_PARAMETERS*/) {
    this->cutoff = parameters.cutoff;
    this->small_cutoff = parameters.small_cutoff;
    this->lower_pitch_cutoff = parameters.lower_pitch_cutoff;
}


McLeodPitchExtractorMethod::McLeodPitchExtractorMethod(const McLeodPitchExtractorMethod & extractor) {
    this->cutoff = extractor.cutoff;
    this->small_cutoff = extractor.small_cutoff;
    this->lower_pitch_cutoff = extractor.lower_pitch_cutoff;
}


McLeodPitchExtractorMethod::~McLeodPitchExtractorMethod() {
    // Destructor
}


void McLeodPitchExtractorMethod::setCutoff(double cutoff) {
    this->cutoff = cutoff;
}


void McLeodPitchExtractorMethod::setSmallCutoff(double small_cutoff) {
    this->small_cutoff = small_cutoff;
}


void McLeodPitchExtractorMethod::setLowerPitch(double lower_pitch_cutoff) {
    this->lower_pitch_cutoff = lower_pitch_cutoff;
}


double McLeodPitchExtractorMethod::getCutoff() const {
    return cutoff;
}


double McLeodPitchExtractorMethod::getSmallCutoff() const {
    return small_cutoff;
}


double McLeodPitchExtractorMethod::getLowerPitch() const {
    return lower_pitch_cutoff;
}


void McLeodPitchExtractorMethod::normalizedSquareDifference(const std::vector<double> & audio_buffer, std::vector<double> & nsdf) const {
    // Calculate Mean value
    double mean_buffer = std::accumulate(audio_buffer.begin(), audio_buffer.end(), 0.0) / audio_buffer.size();
    nsdf = std::vector<double>(audio_buffer.size());
    double maxval;
    double acf = 0.0;
    for(size_t tau = 0; tau < audio_buffer.size(); tau++) {
        acf = 0.0;
        for(size_t i = 0; i < audio_buffer.size() - tau; i++) {
            acf += (audio_buffer[i] - mean_buffer) * (audio_buffer[i + tau] - mean_buffer);
        }
        if(tau == 0) {
            maxval = acf;
        }
        nsdf[tau] = acf / maxval;
    }
}

double McLeodPitchExtractorMethod::get_pitch(const std::vector<double> & audio_buffer, double sample_rate_hz) const {
    // 0. Clear previous results (Is this faster than initializing a list
    // again and again?)
    std::vector<unsigned int> max_positions;
    std::vector<double> period_estimates;
    std::vector<double> amp_estimates;
    std::vector<double> nsdf;
    double turningpoint_x;
    double turningpoint_y;
    // 1. Calculate the normalized square difference for each Tau value.
    this->normalizedSquareDifference(audio_buffer, nsdf);
    // 2. Peak picking time: time to pick some peaks.
    this->peakPicking(nsdf, max_positions);
    double highest_amplitude = -std::numeric_limits<double>::infinity();
    for(unsigned int i = 0; i < max_positions.size(); i++) {
        unsigned int tau = max_positions[i];
        // make sure every annotation has a probability attached
        highest_amplitude = std::max(highest_amplitude, nsdf[tau]);
        if(nsdf[tau] > this->small_cutoff) {
            // calculates turningpoint_x and Y
            this->parabolicInterpolation(tau, nsdf, turningpoint_x, turningpoint_y);
            // store the turning points
            amp_estimates.push_back(turningpoint_y);
            period_estimates.push_back(turningpoint_x);
            // remember the highest amplitude
            highest_amplitude = std::max(highest_amplitude, turningpoint_y);
        }
    }
    double pitch = std::numeric_limits<double>::quiet_NaN();
    if(!period_estimates.empty()) {
        // use the overall maximum to calculate a cutoff.
        // The cutoff value is based on the highest value and a relative
        // threshold.
        double actualCutoff = this->cutoff * highest_amplitude;
        // find first period above or equal to cutoff
        unsigned int periodIndex = 0;
        for(unsigned int i = 0; i < amp_estimates.size(); i++) {
            if(amp_estimates[i] >= actualCutoff) {
                periodIndex = i;
                break;
            }
        }
        double period = period_estimates[periodIndex];
        double pitch_estimate = sample_rate_hz / period;
        if(pitch_estimate > this->lower_pitch_cutoff) {
            pitch = pitch_estimate;
        }
    }
    return pitch;
}


void McLeodPitchExtractorMethod::parabolicInterpolation(unsigned int tau, const std::vector<double> & nsdf, double & turningpoint_x, double & turningpoint_y) const {
    double nsdfa = nsdf[tau - 1];
    double nsdfb = nsdf[tau];
    double nsdfc = nsdf[tau + 1];
    unsigned int bvalue = tau;
    double bottom = nsdfc + nsdfa - 2 * nsdfb;
    if(bottom == 0.0) {
        turningpoint_x = bvalue;
        turningpoint_y = nsdfb;
    } else {
        double delta = nsdfa - nsdfc;
        turningpoint_x = bvalue + delta / (2 * bottom);
        turningpoint_y = nsdfb - delta * delta / (8 * bottom);
    }
}


void McLeodPitchExtractorMethod::peakPicking(const std::vector<double> & nsdf, std::vector<unsigned int> & max_positions) const {
    unsigned int pos = 0;
    unsigned int cur_max_pos = 0;
    // find the first negative zero crossing
    while(pos < (nsdf.size() - 1) / 3 && nsdf[pos] > 0) {
        pos++;
    }
    // loop over all the values below zero
    while(pos < nsdf.size() - 1 && nsdf[pos] <= 0.0) {
        pos++;
    }
    // can happen if output[0] is NAN
    if(pos == 0) {
        pos = 1;
    }
    while(pos < nsdf.size() - 1) {
        if(nsdf[pos] > nsdf[pos - 1] && nsdf[pos] >= nsdf[pos + 1] ) {
            if(cur_max_pos == 0) {
                // the first max (between zero crossings)
                cur_max_pos = pos;
            } else if(nsdf[pos] > nsdf[cur_max_pos]) {
                // a higher max (between the zero crossings)
                cur_max_pos = pos;
            }
        }
        pos++;
        // a negative zero crossing
        if(pos < nsdf.size() - 1 && nsdf[pos] <= 0) {
            // if there was a maximum add it to the list of maxima
            if(cur_max_pos > 0) {
                max_positions.push_back(cur_max_pos);
                cur_max_pos = 0; // clear the maximum position, so we start
                // looking for a new ones
            }
            while(pos < nsdf.size() - 1 && nsdf[pos] <= 0.0) {
                pos++; // loop over all the values below zero
            }
        }
    }
    if(cur_max_pos > 0) {
        // if there was a maximum in the last part
        max_positions.push_back(cur_max_pos); // add it to the vector of maxima
    }
}
