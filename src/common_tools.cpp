#ifdef _MSC_VER
    // Remove warning of fopen
    #define _CRT_SECURE_NO_WARNINGS
    // M_PI constant
    #define _USE_MATH_DEFINES
#endif
#include "common_tools.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <limits>


bool exists(const std::string & name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}


uint64_t count_nan(const std::vector<double> & signal) {
    uint64_t nb_nan = 0;
    for(const auto & sample : signal) {
        if(std::isnan(sample)) {
            nb_nan++;
        }
    }
    return nb_nan;
}


void erase_nan(std::vector<double> & signal) {
    for(int64_t k = signal.size() - 1; k >= 0; k--) {
        if(std::isnan(signal[k])) {
            signal.erase(signal.begin() + k);
        }
    }
}


std::vector<bool> mask_logical_or(const std::vector<bool> & maskA, const std::vector<bool> & maskB) {
    if(maskA.size() != maskB.size()) {
        throw std::runtime_error("Input mask sizes are not equal");
    }
    std::vector<bool> OR_mask;
    for(size_t k = 0; k < maskA.size(); k++) {
        OR_mask.push_back(maskA[k] || maskB[k]);
    }
    return OR_mask;
}


std::vector<double> median_filter(const std::vector<double> & signal, uint64_t kernel_size) {
    double nan = std::numeric_limits<double>::quiet_NaN();
    // Size of the input signal
    uint64_t N = signal.size();
    // Kernel size must be an odd number
    if((kernel_size % 2) == 0) {
        throw std::runtime_error("Kernel size of the median filer has to be an odd number");
    }
    // Minimum kernel size
    if(kernel_size == 1) {
        return signal;
    }
    // Temporary vector to sort
    std::vector<double> temp_array(kernel_size, 0);
    // Indice of the median value of the temporary array
    uint64_t kMiddle = (kernel_size - 1) / 2;
    // Vector storing the result filtered signal
    std::vector<double> signal_filtered;
    for(uint64_t k = 0; k < signal.size(); k++) {
        if(k < kMiddle) {
            temp_array.assign(signal.begin(), signal.begin() + kernel_size);
        } else if(k > N - kMiddle) {
            temp_array.assign(signal.end() - kernel_size, signal.end());
        } else {
            temp_array.assign(signal.begin() + k - kMiddle, signal.begin() + k + kMiddle + 1);
        }
        uint64_t nb_nan = count_nan(temp_array);
        if(nb_nan > kMiddle) {
            signal_filtered.push_back(nan);
        } else if(nb_nan > 0) {
            erase_nan(temp_array);
            uint64_t kMiddleTemp = (temp_array.size()-1)/2;
            std::sort(temp_array.begin(), temp_array.end());
            signal_filtered.push_back(temp_array[kMiddleTemp]);
        } else {
            std::sort(temp_array.begin(), temp_array.end());
            signal_filtered.push_back(temp_array[kMiddle]);
        }
    }
    return signal_filtered;
}


double autocovariance(const std::vector<double> & data){
    if(data.size() < 2) {
        throw std::runtime_error("Cannot calculate autocovariance of data with length < 2");
    }
    uint64_t N = data.size();
    double Xs = 0;
    for (auto& sample : data) {
        Xs += sample;
    }
    Xs = Xs/(double)N;
    double autoCov = 0;
    for (auto& sample : data) {
        autoCov += pow((sample-Xs), 2.0);
    }
    return (1.0/(N-1.0))*autoCov;
}


// GAUSSIAN KDE
/////////////////////////////////////////////////////////////////////
GaussianKde::GaussianKde(const std::vector<double> & dataset, double bw_method) {
    this->dataset = dataset;
    this->n = dataset.size();
    this->factor = bw_method;
    this->computeCovariance();
}


GaussianKde::GaussianKde(const std::vector<double> & dataset, const std::string & bw_method/*="scott"*/) {
    this->dataset = dataset;
    this->n = dataset.size();
    if(bw_method.compare("scott")) {
        this->factor = pow((double)this->n, -1.0 / (1 + 4));
    } else if(bw_method.compare("silverman")) {
        this->factor = pow((double)this->n * (1 + 2.0) / 4.0, -1.0 / (1 + 4));
    } else {
        throw std::logic_error("Invalid gaussian KDE method or not implemented");
    }
    this->computeCovariance();
}


void GaussianKde::computeCovariance() {
    this->_data_covariance = autocovariance(this->dataset);
    this->_data_inv_cov = 1.0/this->_data_covariance;
    this->covariance = this->_data_covariance * pow(this->factor, 2);
    this->inv_cov = this->_data_inv_cov / pow(this->factor, 2);
    this->_norm_factor = sqrt(2 * M_PI * this->covariance) * this->n;
}


GaussianKde::~GaussianKde() {
    // Destructor
}


std::vector<double> GaussianKde::evaluate(const std::vector<double> & points) {
    uint64_t m = points.size();
    std::vector<double> result(m, 0);
    double diff;
    double tdiff;
    double energy;
    for(uint64_t k = 0; k < this->n; k++) {
      for(uint64_t p = 0; p < m; p++) {
        diff = this->dataset[k] - points[p];
        tdiff = this->inv_cov * diff;
        energy = diff * tdiff / 2.0;
        result[p] = result[p] + exp(-energy);
      }
    }
    for(uint64_t p = 0; p <m ; p++) {
      result[p] = result[p] / this->_norm_factor ;
    }
    return result;
}
/////////////////////////////////////////////////////////////////////

std::vector<double> get_histogram_points(const std::vector<double> & signal, uint64_t bins) {
    if(signal.size() <= 1) {
        throw std::runtime_error("input signal should have multiple elements");
    }
    double xmin = std::numeric_limits<double>::max();
    double xmax = -std::numeric_limits<double>::max();
    for(auto& sample : signal) {
        if(sample > xmax) {
            xmax = sample;
        }
        if(sample < xmin) {
            xmin = sample;
        }
    }
    std::vector<double> points;
    double stepbins = (xmax-xmin)/(double)bins;
    for(uint64_t k = 0; k <= bins; k++) {
        points.push_back(xmin + (double)stepbins * k);
    }
    return points;
}


// classic Histogram
Histogram histogram(const std::vector<double> & data, uint64_t bins, double range_min, double range_max) {
    if(data.size() <= 1) {
        throw std::runtime_error("input data should have multiple elements");
    }
    double stepbins = (range_max - range_min) / (double)bins;
    std::vector<double> data_sorted = data;
    std::sort(data_sorted.begin(), data_sorted.end());
    // Initialization
    size_t pmin = 0;
    Histogram result;
    result.x.push_back(range_min);
    result.data = std::vector<double>(bins, 0);
    for (size_t k = 0; k < bins; k++) {
        result.x.push_back(range_min + stepbins*(k + 1));
        for(size_t p = pmin; p < data_sorted.size(); p++) {
            if((k == bins - 1) && (data_sorted[p] == result.x[k + 1])) {
                result.data[k] = result.data[k] + 1.0;
            } else if(data_sorted[p] >= result.x[k + 1]) {
                pmin = p;
                break;
            } else if((data_sorted[p] >= result.x[k]) && (data_sorted[p] < result.x[k + 1])) {
                result.data[k] = result.data[k] + 1.0;
            }
        }
    }
    return result;
}


// Convert tone to frequency and vice versa
double convert_tone_to_freq(double tone_st, double f0_hz/*=F0_HZ*/) {
    if(std::isnan(tone_st)) {
        return tone_st;
    }
    return(f0_hz * pow(2.0, tone_st / 12.0));
}


double convert_freq_to_tone(double freq_hz, double f0_hz/*=F0_HZ*/) {
    if(std::isnan(freq_hz)) {
        return freq_hz;
    }
    return(log10(freq_hz / f0_hz) * 12.0 / log10(2.0));
}


std::vector<double> gaussian(uint64_t nb_samples, double standard_deviation, bool density_function/*=false*/) {
    if((nb_samples % 2) == 0) {
        throw std::runtime_error("The number of samples must be an odd number");
    }
    uint64_t ind_middle = (nb_samples - 1) / 2;
    double coef = 1.0;
    if(density_function) {
        coef = 1.0 / (standard_deviation * sqrt(2.0 * M_PI));
    }
    std::vector<double> result;
    for (uint64_t ind = 0; ind < nb_samples; ind++) {
        result.push_back(coef * exp(-0.5 * pow(((double)ind - (double)ind_middle) / standard_deviation, 2.0)));
    }
    return result;
}
