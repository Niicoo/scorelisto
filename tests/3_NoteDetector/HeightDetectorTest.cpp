#include <gtest/gtest.h>
#include "HeightDetector.hpp"
#include <tuple>



enum class ScaleType {
    MAJOR,
    MINOR_NATURAL,
    MINOR_HARMONIC,
    MINOR_MELODIC_UP,
    MINOR_MELODIC_DOWN
};


std::vector<int> buildScale(ScaleType type, KeyFifths fifth) {
    std::tuple<Step, Alter> key;
    if(type == ScaleType::MAJOR) {
        key = getMajorKey(fifth);
    } else {
        key = getMinorKey(fifth);
    }
    int height_start = static_cast<int>(std::get<0>(key)) + static_cast<int>(std::get<1>(key));
    std::vector<int> scale = {height_start};
    switch(type){
        case ScaleType::MAJOR:
            for(const int interval: {2, 2, 1, 2, 2, 2, 1}) {
                scale.push_back(scale.back() + interval);
            }
            break;
        case ScaleType::MINOR_NATURAL:
            for(const int interval: {2, 1, 2, 2, 1, 2, 2}) {
                scale.push_back(scale.back() + interval);
            }
            break;
        case ScaleType::MINOR_HARMONIC:
            for(const int interval: {2, 1, 2, 2, 1, 3, 1}) {
                scale.push_back(scale.back() + interval);
            }
            break;
        case ScaleType::MINOR_MELODIC_UP:
            for(const int interval: {2, 1, 2, 2, 2, 2, 1}) {
                scale.push_back(scale.back() + interval);
            }
            break;
        case ScaleType::MINOR_MELODIC_DOWN:
            for(const int interval: {2, 2, 1, 2, 2, 1, 2}) {
                scale.push_back(scale.back() - interval);
            }
            break;
        default:
            throw std::runtime_error("Input scale option does not exists");
    }
    return scale;
}

TEST(HeightDetectorTest, Basic) {
    // Considering f0 = 32.7032 hz
    HeightDetector heightDetector;
    std::vector<KeyFifths> FifthsToTest = { KeyFifths::ZERO,
                                            KeyFifths::ONE_SHARP,
                                            KeyFifths::TWO_SHARPS,
                                            KeyFifths::THREE_SHARPS,
                                            KeyFifths::FOUR_SHARPS,
                                            KeyFifths::FIVE_SHARPS,
                                            KeyFifths::SIX_SHARPS,
                                            KeyFifths::SEVEN_SHARPS,
                                            KeyFifths::ONE_FLAT,
                                            KeyFifths::TWO_FLATS,
                                            KeyFifths::THREE_FLATS,
                                            KeyFifths::FOUR_FLATS,
                                            KeyFifths::FIVE_FLATS,
                                            KeyFifths::SIX_FLATS,
                                            KeyFifths::SEVEN_FLATS};

    std::vector<ScaleType> ScaleToTest = {  ScaleType::MAJOR,
                                            ScaleType::MINOR_NATURAL,
                                            ScaleType::MINOR_HARMONIC,
                                            ScaleType::MINOR_MELODIC_UP,
                                            ScaleType::MINOR_MELODIC_DOWN};
    
    for(const auto fifth: FifthsToTest) {
        for(const auto scaletype: ScaleToTest) {
            std::vector<int> scale_int = buildScale(scaletype, fifth);
            std::vector<double> scale(scale_int.begin(), scale_int.end());
            heightDetector.fit(scale);
            KeyFifths fifth_estimated = heightDetector.getFifth();
            KeyMode mode_estimated = heightDetector.getMode();
            ClefSign sign_estimated = heightDetector.getClef();
            EXPECT_EQ(fifth_estimated, fifth);
            if(scaletype == ScaleType::MAJOR) {
                EXPECT_EQ(mode_estimated, KeyMode::MAJOR);
            } else {
                EXPECT_EQ(mode_estimated, KeyMode::MINOR);
            }
            EXPECT_EQ(sign_estimated, ClefSign::G);
        }
    }
}
