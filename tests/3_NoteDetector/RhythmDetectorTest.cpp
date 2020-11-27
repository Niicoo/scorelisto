#include <gtest/gtest.h>
#include "RhythmDetector.hpp"
#include "StepDetector.hpp"

double getquarterlength(double bpm) {
    return 60.0 / bpm;
}


bool operator==(const NoteRhythm & lhs, const NoteRhythm & rhs) {
    if(lhs.duration != rhs.duration) {
        return false;
    }
    if(lhs.type != rhs.type) {
        return false;
    }
    if(lhs.dot != rhs.dot) {
        return false;
    }
    if(lhs.triplet != rhs.triplet) {
        return false;
    }
    if(lhs.tie_start != rhs.tie_start) {
        return false;
    }
    if(lhs.tie_stop != rhs.tie_stop) {
        return false;
    }
    if(lhs.beams.size() != rhs.beams.size()) {
        return false;
    }
    for(auto const & beam: lhs.beams) {
        if(rhs.beams.count(beam.first) <= 0) {
            return false;
        }
        if(beam.second != rhs.beams.at(beam.first)) {
            return false;
        }
    }
    if(lhs.slurs.size() != rhs.slurs.size()) {
        return false;
    }
    for(auto const & slur: lhs.slurs) {
        if(rhs.slurs.count(slur.first) <= 0) {
            return false;
        }
        if(slur.second.placement != rhs.slurs.at(slur.first).placement) {
            return false;
        }
        if(slur.second.type != rhs.slurs.at(slur.first).type) {
            return false;
        }
    }
    return true;
}

TEST(RhythmDetectorTest, Basic) {
    StepResult step_result_tested;
    RhythmDetector rhythm_detector;
    rhythm_detector.fit(step_result_tested);
    EXPECT_ANY_THROW(rhythm_detector.perform(nullptr));
    double bpm = 95.0;
    double ql = getquarterlength(bpm);
    // 1NOTE_1BEAT
    step_result_tested.notes.push_back({true, ql, 9.0, 0.5, false});
    // EN_EN
    step_result_tested.notes.push_back({true, ql/2.0, 9.0, 0.5, false});
    step_result_tested.notes.push_back({true, ql/2.0, 9.0, 0.5, false});
    // T_EN_EN_EN
    step_result_tested.notes.push_back({true, ql/3.0, 9.0, 0.5, false});
    step_result_tested.notes.push_back({true, ql/3.0, 9.0, 0.5, false});
    step_result_tested.notes.push_back({true, ql/3.0, 9.0, 0.5, false});
    // 1NOTE_2BEAT
    step_result_tested.notes.push_back({true, ql*2, 9.0, 0.5, false});
    // 1NOTE_1BEAT
    step_result_tested.notes.push_back({true, ql, 9.0, 0.5, false});
    // DQN_EN
    step_result_tested.notes.push_back({true, ql*1.5, 9.0, 0.5, false});
    step_result_tested.notes.push_back({true, ql/2.0, 9.0, 0.5, false});
    // SN_SN_SN_SN
    step_result_tested.notes.push_back({true, ql/4.0, 9.0, 0.5, false});
    step_result_tested.notes.push_back({true, ql/4.0, 9.0, 0.5, false});
    step_result_tested.notes.push_back({true, ql/4.0, 9.0, 0.5, false});
    step_result_tested.notes.push_back({true, ql/4.0, 9.0, 0.5, false});
    // EN_SN_SN
    step_result_tested.notes.push_back({true, ql/2.0, 9.0, 0.5, false});
    step_result_tested.notes.push_back({true, ql/4.0, 9.0, 0.5, false});
    step_result_tested.notes.push_back({true, ql/4.0, 9.0, 0.5, false});
    // SN_SN_EN
    step_result_tested.notes.push_back({true, ql/4.0, 9.0, 0.5, false});
    step_result_tested.notes.push_back({true, ql/4.0, 9.0, 0.5, false});
    step_result_tested.notes.push_back({true, ql/2.0, 9.0, 0.5, false});
    // T_SN_EN_DEN
    step_result_tested.notes.push_back({true, ql/6.0, 9.0, 0.5, false});
    step_result_tested.notes.push_back({true, ql/3.0, 9.0, 0.5, false});
    step_result_tested.notes.push_back({true, ql/2.0, 9.0, 0.5, false});
    // 1NOTE_6BEAT
    step_result_tested.notes.push_back({true, ql*6.0, 9.0, 0.5, false});

    rhythm_detector.fit(step_result_tested);
    RhythmResult rhythms = rhythm_detector.perform(nullptr);
    // 1NOTE_1BEAT
    NoteRhythm note_expected = {LENGTH_QUARTER, Type::QUARTER, false, false, {}, {}, false, false};
    EXPECT_EQ(rhythms[0].second, note_expected);
    // EN_EN
    note_expected = {LENGTH_EIGHTH, Type::EIGHTH, false, false, {{1, BeamOption::BEGIN}}, {}, false, false};
    EXPECT_EQ(rhythms[1].second, note_expected);
    note_expected = {LENGTH_EIGHTH, Type::EIGHTH, false, false, {{1, BeamOption::END}}, {}, false, false};
    EXPECT_EQ(rhythms[2].second, note_expected);
    // T_EN_EN_EN
    note_expected = {LENGTH_T_EIGHTH, Type::EIGHTH, false, true, {{1, BeamOption::BEGIN}}, {}, false, false};
    EXPECT_EQ(rhythms[3].second, note_expected);
    note_expected = {LENGTH_T_EIGHTH, Type::EIGHTH, false, true, {{1, BeamOption::CONTINUE}}, {}, false, false};
    EXPECT_EQ(rhythms[4].second, note_expected);
    note_expected = {LENGTH_T_EIGHTH, Type::EIGHTH, false, true, {{1, BeamOption::END}}, {}, false, false};
    EXPECT_EQ(rhythms[5].second, note_expected);
    // 1NOTE_2BEAT
    note_expected = {LENGTH_HALF, Type::HALF, false, false, {}, {}, false, false};
    EXPECT_EQ(rhythms[6].second, note_expected);
    // 1NOTE_1BEAT
    note_expected = {LENGTH_QUARTER, Type::QUARTER, false, false, {}, {}, false, false};
    EXPECT_EQ(rhythms[7].second, note_expected);
    // DQN_EN
    note_expected = {LENGTH_QUARTER + LENGTH_EIGHTH, Type::QUARTER, true, false, {}, {}, false, false};
    EXPECT_EQ(rhythms[8].second, note_expected);
    note_expected = {LENGTH_EIGHTH, Type::EIGHTH, false, false, {}, {}, false, false};
    EXPECT_EQ(rhythms[9].second, note_expected);
    // SN_SN_SN_SN
    note_expected = {LENGTH_16TH, Type::SIXTEENTH, false, false, {{1, BeamOption::BEGIN}, {2, BeamOption::BEGIN}}, {}, false, false};
    EXPECT_EQ(rhythms[10].second, note_expected);
    note_expected = {LENGTH_16TH, Type::SIXTEENTH, false, false, {{1, BeamOption::CONTINUE}, {2, BeamOption::CONTINUE}}, {}, false, false};
    EXPECT_EQ(rhythms[11].second, note_expected);
    note_expected = {LENGTH_16TH, Type::SIXTEENTH, false, false, {{1, BeamOption::CONTINUE}, {2, BeamOption::CONTINUE}}, {}, false, false};
    EXPECT_EQ(rhythms[12].second, note_expected);
    note_expected = {LENGTH_16TH, Type::SIXTEENTH, false, false, {{1, BeamOption::END}, {2, BeamOption::END}}, {}, false, false};
    EXPECT_EQ(rhythms[13].second, note_expected);
    // EN_SN_SN
    note_expected = {LENGTH_EIGHTH, Type::EIGHTH, false, false, {{1, BeamOption::BEGIN}}, {}, false, false};
    EXPECT_EQ(rhythms[14].second, note_expected);
    note_expected = {LENGTH_16TH, Type::SIXTEENTH, false, false, {{1, BeamOption::CONTINUE}, {2, BeamOption::BEGIN}}, {}, false, false};
    EXPECT_EQ(rhythms[15].second, note_expected);
    note_expected = {LENGTH_16TH, Type::SIXTEENTH, false, false, {{1, BeamOption::END}, {2, BeamOption::END}}, {}, false, false};
    EXPECT_EQ(rhythms[16].second, note_expected);
    // SN_SN_EN
    note_expected = {LENGTH_16TH, Type::SIXTEENTH, false, false, {{1, BeamOption::BEGIN}, {2, BeamOption::BEGIN}}, {}, false, false};
    EXPECT_EQ(rhythms[17].second, note_expected);
    note_expected = {LENGTH_16TH, Type::SIXTEENTH, false, false, {{1, BeamOption::CONTINUE}, {2, BeamOption::END}}, {}, false, false};
    EXPECT_EQ(rhythms[18].second, note_expected);
    note_expected = {LENGTH_EIGHTH, Type::EIGHTH, false, false, {{1, BeamOption::END}}, {}, false, false};
    EXPECT_EQ(rhythms[19].second, note_expected);
    // T_SN_EN_DEN
    note_expected = {LENGTH_T_16TH, Type::SIXTEENTH, false, true, {{1, BeamOption::BEGIN}}, {}, false, false};
    EXPECT_EQ(rhythms[20].second, note_expected);
    note_expected = {LENGTH_T_EIGHTH, Type::EIGHTH, false, true, {{1, BeamOption::CONTINUE}}, {}, false, false};
    EXPECT_EQ(rhythms[21].second, note_expected);
    note_expected = {LENGTH_T_EIGHTH + LENGTH_T_16TH, Type::EIGHTH, true, true, {{1, BeamOption::END}}, {}, false, false};
    EXPECT_EQ(rhythms[22].second, note_expected);
    // 1NOTE_6BEAT
    note_expected = {LENGTH_HALF + LENGTH_QUARTER, Type::HALF, true, false, {}, {}, true, false};
    EXPECT_EQ(rhythms[23].second, note_expected);
    note_expected = {LENGTH_HALF + LENGTH_QUARTER, Type::HALF, true, false, {}, {}, false, true};
    EXPECT_EQ(rhythms[24].second, note_expected);
}
