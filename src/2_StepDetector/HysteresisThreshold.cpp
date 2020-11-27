#include "HysteresisThreshold.hpp"
#include <vector>
#include <iostream>
#include <cmath>


HysteresisThreshold::HysteresisThreshold(double DeactivationThreshold, double ActivationThreshold, bool mode_mask/*=false*/, bool quick_activation/*=false*/, bool quick_deactivation/*=false*/) {
    if(ActivationThreshold <= DeactivationThreshold) {
        throw std::runtime_error("Activation threshold is lower than DeActivation threshold");
    }
    this->deactivation = DeactivationThreshold;
    this->activation = ActivationThreshold;
    // When mode_mask is true, activated state is false and deactivated state is true
    this->mode_mask = mode_mask;
    this->activated_state = !mode_mask;
    this->deactivated_state = mode_mask;
    this->quick_activation = quick_activation;
    this->quick_deactivation = quick_deactivation;
}


HysteresisThreshold::~HysteresisThreshold() {
    // Destructor
}


// NaN are then considered as below deactivation threshold
int HysteresisThreshold::getState(double value) const {
    if(value > this->activation) {
        // Above the activation threshold
        return 2;
    } else if((value > this->deactivation) || std::isnan(value)){
        // Between the both thresholds
        return 1;
    } else {
        // Below the deactivation threshold
        return 0;
    }
}


std::vector<bool> HysteresisThreshold::perform(const std::vector<double> & signal) const {
    // Size of the input signal
    uint64_t N = signal.size();
    // Prepare the result vector
    std::vector<bool> result(N, this->deactivated_state);
    if(N==0) {
        return result;
    }
    // Initial condition
    int past_state = this->getState(signal[0]);
    uint64_t k1start = 0;
    int state_before_state_1;
    if(past_state == 0) {
        result[0] = this->deactivated_state;
    } else if(past_state == 1) {
        k1start = 0;
        state_before_state_1 = 0;
    } else if(past_state == 2) {
        result[0] = this->activated_state;
    }
    int current_state;
    for(uint64_t k=1; k < N; k++) {
        current_state = this->getState(signal[k]);
        if(current_state == 0) {
            result[k] = this->deactivated_state;
            if(past_state == 1) {
                if((state_before_state_1 == 0) || this->quick_deactivation) {
                    std::fill(result.begin()+k1start, result.begin() + k, this->deactivated_state);
                } else {
                    std::fill(result.begin()+k1start, result.begin() + k, this->activated_state);
                }
            }
            past_state = 0;
        } else if(current_state == 1) {
            if(past_state == 0){
                k1start = k;
                state_before_state_1 = 0;
            } else if(past_state == 2) {
                k1start = k;
                state_before_state_1 = 2;
            }
            past_state = 1;
        } else if(current_state == 2) {
            result[k] = this->activated_state;
            if(past_state == 1) {
                if((state_before_state_1 == 2) || this->quick_activation) {
                    std::fill(result.begin()+k1start, result.begin() + k, this->activated_state);
                } else {
                    std::fill(result.begin()+k1start, result.begin() + k, this->deactivated_state);
                }
            }
            past_state = 2;
        }
    }
    // Conditions at the end of the signal
    if(past_state == 1) {
        if(state_before_state_1 == 0) {
            std::fill(result.begin()+k1start, result.end(), this->deactivated_state);
        } else if(state_before_state_1 == 2) {
            std::fill(result.begin()+k1start, result.end(), this->activated_state);
        }
    }
    return result;
}
