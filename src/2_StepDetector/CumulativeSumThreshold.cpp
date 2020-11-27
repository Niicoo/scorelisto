#include "CumulativeSumThreshold.hpp"
#include <cstdint>
#include <vector>


CumulativeSumThreshold::CumulativeSumThreshold(double threshold_min, double cumsum_min, bool mode_mask) {
    this->threshold_min = threshold_min;
    this->cumsum_min = cumsum_min;
    this->mode_mask = mode_mask;
    this->activated_state = !mode_mask;
    this->deactivated_state = mode_mask;
}


CumulativeSumThreshold::~CumulativeSumThreshold() {
    // Destructor
}


std::vector<bool> CumulativeSumThreshold::perform(const std::vector<double> & signal) const {
    // Size of the input signal
    uint64_t N = signal.size();
    // Prepare the result vector
    std::vector<bool> result(N, this->deactivated_state);
    if(N==0) {
        return result;
    }
    bool past_state = signal[0] >= this->threshold_min;
    uint64_t k_start = 0;
    double cumsum = 0.0;
    if(past_state) {
        cumsum = signal[0];
    }
    
    bool current_state;
    for(uint64_t k=1; k < N; k++) {
        current_state = signal[k] >=  this->threshold_min;
        if(current_state) {
            if(!past_state) {
                k_start = k;
                cumsum = 0;
            }
            cumsum += signal[k];
        } else if(past_state){
            if(cumsum > this->cumsum_min) {
                std::fill(result.begin() + k_start, result.begin() + k, this->activated_state);
            }
        }
        past_state = current_state;
    }

    if(past_state && cumsum > this->cumsum_min) {
        std::fill(result.begin() + k_start, result.end(), this->activated_state);
    }
    return result;
}
