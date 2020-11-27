#ifndef TOOLS
#define TOOLS

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <tuple>

// Standard frequency for the equally tempered scale
const double F0_HZ = 32.7032;


// Count number of NaN value in the vector
uint64_t count_nan(const std::vector<double> & signal);


// Erase nan samples in a vector
void erase_nan(std::vector<double> & signal);


// Check if the specified file exists and is readable
bool exists(const std::string & name);


// Logical OR on vectors of boolean values
std::vector<bool> mask_logical_or(const std::vector<bool> & maskA, const std::vector<bool> & maskB);


// Median filter
std::vector<double> median_filter(const std::vector<double> & signal, uint64_t kernel_size);


// Calculate the autocovariance of a vector
double autocovariance(const std::vector<double> & data);


// Gaussian kde
class GaussianKde {
public:
    // Constructor
    GaussianKde(const std::vector<double> & dataset, const std::string & bw_method="scott");
    GaussianKde(const std::vector<double> & dataset, double bw_method);
    // Destructor
    virtual ~GaussianKde();
    std::vector<double> evaluate(const std::vector<double> & points);
private:
    void computeCovariance();
    std::vector<double> dataset;
    uint64_t n;
    double factor;
    double _data_covariance;
    double _data_inv_cov;
    double covariance;
    double inv_cov;
    double _norm_factor;
};


// Data of an histogram
struct Histogram {
    std::vector<double> data;
    std::vector<double> x;
};


// Calculate the points to evaluate for histogram
std::vector<double> get_histogram_points(const std::vector<double> & signal, uint64_t bins);


// classic Histogram
Histogram histogram(const std::vector<double> & signal, uint64_t bins, double range_min, double range_max);


double convert_tone_to_freq(double tone_st, double f0_hz=F0_HZ);


double convert_freq_to_tone(double freq_hz, double f0_hz=F0_HZ);


std::vector<double> gaussian(uint64_t nb_samples, double standard_deviation, bool density_function=false);


// Template functions
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

// Count number of NaN value in a vector
template<typename T>
uint64_t count_nan(const std::vector<T> & signal);


// Erase the NaN values inside a vector
template<typename T>
void erase_nan(std::vector<T> & signal);


template<typename T>
std::vector<T> convolve(const std::vector<T> & A, const std::vector<T> & B) {
    // Output is same size as A vector
    size_t ind_middle;
    if(A.size() == B.size()){
        if(A.size() % 2 == 0){
            ind_middle = (A.size() / 2) - 1;
        } else {
            ind_middle = (A.size() - 1) / 2;
        }
    } else {
        if(B.size() % 2 == 0){
            ind_middle = (B.size() / 2) - 1;
        } else{
            ind_middle = (B.size() - 1) / 2;
        }
    }
    std::vector<T> result;
    T temp_value;
    size_t m_min;
    size_t m_max;
    for (size_t n = ind_middle; n < ind_middle + A.size(); n++) {
        temp_value = 0.0;
        m_min = (B.size() > n) ? 0 : (n - B.size() + 1);
        // m_min = std::max((size_t)0, n - B.size() + 1);
        m_max = std::min(A.size() - 1, n);
        for (size_t m = m_min; m < m_max + 1; m++) {
            temp_value += A[m] * B[n-m];
        }
        result.push_back(temp_value);
    }
    return result;
}


template<typename T>
std::vector<uint64_t> extract_local_min_max(const std::vector<T> & signal, bool border=false, bool local_max=false) {
    // Possible difference between 2 consecutive samples
    enum DerivationState {Positive, Negative, Zero};
    DerivationState previous_derivation = Zero;
    DerivationState current_derivation;
    DerivationState derivation_before_zero;
    std::vector<uint64_t> ind_local_min;
    std::vector<uint64_t> ind_local_max;
    // If the signal length is too small
    if(signal.size() < 2) {
        throw std::runtime_error("Cannot extract local min and local max of signal with size < 2"); 
    }
    // Initial conditions
    uint64_t ind_start;
    for(ind_start = 1; ind_start < signal.size(); ind_start++) {
        // Current state of the derivation
        if(signal[ind_start] > signal[ind_start-1]) {
            if(border) {
                ind_local_min.push_back((uint64_t)round((ind_start - 1) / 2.0));
                if(ind_start == signal.size() - 1) {
                    ind_local_max.push_back(ind_start);
                }
            }
            previous_derivation = Positive;
            break;
        } else if(signal[ind_start] < signal[ind_start-1]) {
            if(border) {
                ind_local_max.push_back((uint64_t)round((ind_start - 1) / 2.0));
                if(ind_start == signal.size() - 1) {
                    ind_local_min.push_back(ind_start);
                }
            }
            previous_derivation = Negative;
            break;
        }
    }
    ind_start += 1;
    // Flat signal
    if(previous_derivation == Zero) {
        return ind_local_min;
    }
    uint64_t ind_zero_start;
    for(uint64_t ind = ind_start; ind < signal.size(); ind++) {
        // Current state of the derivation
        if(signal[ind] > signal[ind - 1]) {
            current_derivation = Positive;
        } else if(signal[ind] == signal[ind - 1]) {
            current_derivation = Zero;
        } else {
            current_derivation = Negative;
        }
        if((current_derivation == Zero) && (previous_derivation != Zero)) {
            ind_zero_start = ind - 1;
            derivation_before_zero = previous_derivation;
        } else if(current_derivation == Positive) {
            // Minimum local
            if(previous_derivation == Negative) {
                ind_local_min.push_back(ind - 1);
            } else if((previous_derivation == Zero) && (derivation_before_zero == Negative)) {
                ind_local_min.push_back((uint64_t)round((ind_zero_start + ind - 1) / 2.0));
            }
            if((ind == signal.size() - 1) && border) {
                ind_local_max.push_back(ind);
            }
        } else if(current_derivation == Negative) {
            // Maximum local
            if(previous_derivation == Positive) {
                ind_local_max.push_back(ind - 1);
            } else if((previous_derivation == Zero) && (derivation_before_zero == Positive)) {
                ind_local_max.push_back((uint64_t)round((ind_zero_start + ind - 1) / 2.0));
            }
            if((ind == signal.size() - 1) && border) {
                ind_local_min.push_back(ind);
            }
        }
        previous_derivation = current_derivation;
    }
    if(local_max) {
        return ind_local_max;
    }
    return ind_local_min;
}

template<typename T>
T most_commun_value(const std::vector<T> & data) {
    if(data.empty()) {
        throw std::runtime_error("Empty vector, cannot calculate most commun value");
    }
    std::vector<T> data_sorted = data;
    std::sort(data_sorted.begin(), data_sorted.end());
    size_t count = 1;
    size_t max_count = 0;
    T value = data_sorted[0];
    T max_count_value = value;
    for(size_t ind = 1; ind < data_sorted.size(); ind++) {
        if(data_sorted[ind] == data_sorted[ind - 1]) {
            count += 1;
        } else {
            if(count > max_count) {
                max_count = count;
                max_count_value = value;
            }
            count = 1;
            value = data_sorted[ind];
        }
    }
    if(count > max_count) {
        max_count = count;
        max_count_value = value;
    }
    return max_count_value;
}


#endif /* TOOLS */
