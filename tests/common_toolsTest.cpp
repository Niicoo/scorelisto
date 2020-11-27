#include <gtest/gtest.h>
#include <vector>
#include "common_tools.hpp"
#include <cmath>

// Check if a file exists
TEST(ExistsTest, CheckingFileExists) {
    EXPECT_TRUE(exists("run_tests"));
    EXPECT_FALSE(exists("run_tests_no"));
}

// count_nan
TEST(count_nanTest, BasicsTest) {
    double nan = std::numeric_limits<double>::quiet_NaN();
    std::vector<double> signal = {1, 2, 4, 2, 4, 4, 1, 5};
    uint64_t nb_nan = count_nan(signal);
    EXPECT_EQ(0, nb_nan);
    signal = {1, 2, 4, 2, 4, nan, nan, nan};
    nb_nan = count_nan(signal);
    EXPECT_EQ(3, nb_nan);
    signal = {nan, nan, nan, nan, nan, nan, nan, nan};
    nb_nan = count_nan(signal);
    EXPECT_EQ(8, nb_nan);
    signal = {};
    nb_nan = count_nan(signal);
    EXPECT_EQ(0, nb_nan);
}

// erase_nan
TEST(erase_nanTest, BasicsTest) {
    double nan = std::numeric_limits<double>::quiet_NaN();
    std::vector<double> signal = {1, 2, 4, 2, 4, 4, 1, 5};
    std::vector<double> signal_expected = {1, 2, 4, 2, 4, 4, 1, 5};
    erase_nan(signal);
    EXPECT_EQ(signal_expected, signal);

    signal = {1, 2, 4, 2, 4, nan, nan, nan};
    signal_expected = {1, 2, 4, 2, 4};
    erase_nan(signal);
    EXPECT_EQ(signal_expected, signal);

    signal = {nan, nan, nan, nan, nan, nan, nan, nan};
    signal_expected = {};
    erase_nan(signal);
    EXPECT_EQ(signal_expected, signal);

    signal = {};
    signal_expected = {};
    erase_nan(signal);
    EXPECT_EQ(signal_expected, signal);
}

// MaskLogicalOR
TEST(MaskLogicalORTest, BasicsTest) {
    std::vector<bool> maskA = {true, true, false, false};
    std::vector<bool> maskB = {true, false, true, false};
    std::vector<bool> mask_expected = {true, true, true, false};
    EXPECT_EQ(mask_expected, mask_logical_or(maskA, maskB));
    maskA = {};
    maskB = {};
    mask_expected = {};
    EXPECT_EQ(mask_expected, mask_logical_or(maskA, maskB));
    maskA = {true, true};
    maskB = {false, true, false};
    EXPECT_ANY_THROW(mask_logical_or(maskA, maskB));
}


// autocovariance
TEST(AutoCovarianceTest, SimpleTest) {
    std::vector<double> data = {1, 2, 3, 2, 3, 2, 1, 2, 3, 4, 4, 2};
    double autocov = autocovariance(data);
    EXPECT_TRUE(abs(autocov - 0.9924242424242425) < 1e-3);
    data = {5.9, 8.7};
    autocov = autocovariance(data);
    EXPECT_TRUE(abs(autocov - 3.92) < 1e-3);
    data = {};
    EXPECT_ANY_THROW(autocovariance(data));
    data = {7.9};
    EXPECT_ANY_THROW(autocovariance(data));
}

// GaussianKdeHistogram
// TEST(GaussianKdeHistogramTest, Test) {
//     std::vector<double> data = {1.3, 4, 5.5, 2.3, 4.9, 9, 0.0, 6.4, 3.2, 2.1, 1.0, 1.1, 8.7, 7.0, 8.9, 7.5, 3.2, 8.2, 0.5, 1.1};
//     Histogram histo = gaussian_kde_histogram(data, 10);
//     std::vector<double> data_expected = {0.07555194, 0.09942449, 0.10658844, 0.09916413, 0.08723554, 0.07944751, 0.0784522 , 0.08065296, 0.07820969, 0.06493802};
//     EXPECT_EQ(data_expected.size(), histo.data.size());
//     for( uint64_t k = 0; k < histo.data.size(); k++) {
//         EXPECT_TRUE(abs(histo.data[k] - data_expected[k]) < 1e-3);
//     }
//     std::vector<double> x_expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//     EXPECT_EQ(x_expected.size(), histo.x.size());
//     for( uint64_t k = 0; k < histo.x.size(); k++) {
//         EXPECT_TRUE(abs(histo.x[k] - x_expected[k]) < 1e-3);
//     }
// }


// Median Filter
////////////////////////////////////////////////////////////////////
TEST(MedianFilterTest, NormalCaseTest) {
    std::vector<double> signal = {1, 2, 4, 2, 4, 2, 5, 5, 6, 7, 8, 9, 7, 7, 6, 7, 4, 3, 2, 7, 8, 7};
    // kernel size : 3
    std::vector<double> result = median_filter(signal, 3);
    std::vector<double> result_expected = {2, 2, 2, 4, 2, 4, 5, 5, 6, 7, 8, 8, 7, 7, 7, 6, 4, 3, 3, 7, 7, 7};
    EXPECT_EQ(result_expected, result);
    // kernel size : 5
    result = median_filter(signal, 5);
    result_expected = {2, 2, 2, 2, 4, 4, 5, 5, 6, 7, 7, 7, 7, 7, 7, 6, 4, 4, 4, 7, 7, 7};
    EXPECT_EQ(result_expected, result);
}

TEST(MedianFilterTest, EmptyVectorTest) {
    std::vector<double> signal = {};
    std::vector<double> result = median_filter(signal, 3);
    std::vector<double> result_expected = {};
    EXPECT_EQ(result_expected, result);
}

TEST(MedianFilterTest, NaNTest) {
    double nan = std::numeric_limits<double>::quiet_NaN();
    std::vector<double> signal = {1, 2, 4, 2, 4, nan, nan, nan, 6, 7, 8, 9, 5, nan, 6, 7, 4, 3, 2, 7, 8, 7};
    // kernel size : 5
    std::vector<double> result = median_filter(signal, 5);
    std::vector<double> result_expected = {2, 2, 2, 2, 4, nan, nan, nan, 7, 7, 7, 7, 6, 6, 5, 4, 4, 4, 4, 7, 7, 7};

    for(uint64_t k = 0; k < result_expected.size(); k++) {
        if(std::isnan(result_expected[k])) {
            EXPECT_TRUE(std::isnan(result[k]));
        } else {
            EXPECT_EQ(result_expected[k], result[k]);
        }
    }
}

TEST(MedianFilterTest, OnlyNaNTest) {
    double nan = std::numeric_limits<double>::quiet_NaN();
    std::vector<double> signal = {nan, nan, nan, nan, nan, nan, nan, nan, nan, nan};
    // kernel size : 5
    std::vector<double> result = median_filter(signal, 5);
    std::vector<double> result_expected = {nan, nan, nan, nan, nan, nan, nan, nan, nan, nan};

    for(uint64_t k = 0; k < result_expected.size(); k++) {
        if(std::isnan(result_expected[k])) {
            EXPECT_TRUE(std::isnan(result[k]));
        } else {
            EXPECT_EQ(result_expected[k], result[k]);
        }
    }
}
////////////////////////////////////////////////////////////////////



// uint64_t most_commun_value(const std::vector<uint64_t> & data)
////////////////////////////////////////////////////////////////////
TEST(most_commun_valueTest, Uint64_t_BasicTest) {
    std::vector<uint64_t> data = {1, 1, 2, 7, 9, 5, 3, 4, 2, 1, 8, 7, 6, 4, 3, 2, 1, 0, 1, 1};
    uint64_t mcv = most_commun_value<uint64_t>(data);
    EXPECT_EQ(1, mcv);
}
TEST(most_commun_valueTest, EmptyVectorTest) {
    std::vector<uint64_t> data = {};
    EXPECT_ANY_THROW(most_commun_value<uint64_t>(data));
}
TEST(most_commun_valueTest, singleValueVectorTest) {
    std::vector<uint64_t> data = {8};
    uint64_t mcv = most_commun_value<uint64_t>(data);
    EXPECT_EQ(8, mcv);
}
TEST(most_commun_valueTest, TwoMostCommonValueTest) {
    std::vector<uint64_t> data = {3, 4, 4, 4, 3, 3};
    uint64_t mcv = most_commun_value<uint64_t>(data);
    EXPECT_TRUE((mcv == 3) || (mcv == 4));
};
TEST(most_commun_valueTest, size_t_BasicTest) {
    std::vector<size_t> data = {1, 1, 2, 7, 9, 5, 3, 4, 2, 1, 8, 7, 6, 4, 3, 2, 1, 0, 1, 1};
    size_t mcv = most_commun_value<size_t>(data);
    EXPECT_EQ(1, mcv);
};
TEST(most_commun_valueTest, size_t_BasicTest2) {
    std::vector<size_t> data1(100, 1);
    std::vector<size_t> data2(55, 0);
    data1.insert(data1.end(), data2.begin(), data2.end());
    size_t mcv = most_commun_value<size_t>(data1);
    EXPECT_EQ(1, mcv);
};
////////////////////////////////////////////////////////////////////
