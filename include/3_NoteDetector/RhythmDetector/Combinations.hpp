#ifndef COMBINATION
#define COMBINATION

#include <vector>
#include <map>
#include "MusicXmlScore.hpp"


static const unsigned int COMBINATIONS_DIVISION = 24;

static const unsigned int LENGTH_WHOLE = COMBINATIONS_DIVISION * 4;
static const unsigned int LENGTH_HALF = COMBINATIONS_DIVISION * 2;
static const unsigned int LENGTH_QUARTER = COMBINATIONS_DIVISION;
static const unsigned int LENGTH_EIGHTH = COMBINATIONS_DIVISION / 2;
static const unsigned int LENGTH_16TH = COMBINATIONS_DIVISION / 4;
static const unsigned int LENGTH_T_EIGHTH = COMBINATIONS_DIVISION / 3;
static const unsigned int LENGTH_T_16TH = COMBINATIONS_DIVISION / 6;


struct Combination {
    Combination(std::vector<unsigned int> durations,
                std::vector<bool> types,
                std::vector<Beams> beams,
                unsigned int nb_notes,
                unsigned int nb_beats,
                bool triplet) {
        this->durations = durations;
        this->types = types;
        this->beams = beams;
        this->nb_notes = nb_notes;
        this->nb_beats = nb_beats;
        this->triplet = triplet;
    }
    std::vector<unsigned int> durations;
    std::vector<bool> types;
    std::vector<Beams> beams;
    unsigned int nb_notes;
    unsigned int nb_beats;
    bool triplet;
};


static const std::map<std::string, Combination> COMBINATIONS = {
    // 1 NOTE
    ////////////////////////////////////////////////////////////////////////////////////////////
    {"1NOTE_1BEAT", Combination(
        {LENGTH_QUARTER}, // durations
        {true}, // types
        {{}}, // beams
        1, // nb_notes
        1, // nb_beats
        false)}, // triplet
    {"1REST_1BEAT", Combination(
        {LENGTH_QUARTER},
        {false},
        {{}},
        1, 1, false)},
    {"1NOTE_2BEATS", Combination(
        {LENGTH_HALF},
        {true},
        {{}},
        1, 2, false)},
    {"1REST_2BEATS", Combination(
        {LENGTH_HALF},
        {false},
        {{}},
        1, 2, false)},
    {"1NOTE_3BEATS", Combination(
        {LENGTH_HALF + LENGTH_QUARTER},
        {true},
        {{}},
        1, 3, false)},
    {"1REST_3BEATS", Combination(
        {LENGTH_HALF + LENGTH_QUARTER},
        {false},
        {{}},
        1, 3, false)},
    {"1NOTE_4BEATS", Combination(
        {LENGTH_WHOLE},
        {true},
        {{}},
        1, 4, false)},
    {"1REST_4BEATS", Combination(
        {LENGTH_WHOLE},
        {false},
        {{}},
        1, 4, false)},
    {"1NOTE_5BEATS", Combination(
        {LENGTH_WHOLE + LENGTH_QUARTER},
        {true},
        {{}},
        1, 5, false)},
    {"1REST_5BEATS", Combination(
        {LENGTH_WHOLE + LENGTH_QUARTER},
        {false},
        {{}},
        1, 5, false)},
    {"1NOTE_6BEATS", Combination(
        {LENGTH_WHOLE + LENGTH_HALF},
        {true},
        {{}},
        1, 6, false)},
    {"1REST_6BEATS", Combination(
        {LENGTH_WHOLE + LENGTH_HALF},
        {false},
        {{}},
        1, 6, false)},
    {"1NOTE_7BEATS", Combination(
        {LENGTH_WHOLE + LENGTH_HALF + LENGTH_QUARTER},
        {true},
        {{}},
        1, 7, false)},
    {"1REST_7BEATS", Combination(
        {LENGTH_WHOLE + LENGTH_HALF + LENGTH_QUARTER},
        {false},
        {{}},
        1, 7, false)},
    {"1NOTE_8BEATS", Combination(
        {LENGTH_WHOLE + LENGTH_WHOLE},
        {true},
        {{}},
        1, 8, false)},
    {"1REST_8BEATS", Combination(
        {LENGTH_WHOLE + LENGTH_WHOLE},
        {false},
        {{}},
        1, 8, false)},
    // 2 NOTES
    ////////////////////////////////////////////////////////////////////////////////////////////
    // 1 BEAT
    {"EN_EN", Combination(
        {LENGTH_EIGHTH, LENGTH_EIGHTH},
        {true, true},
        {{std::make_pair(1, BeamOption::BEGIN)}, {std::make_pair(1, BeamOption::END)}},
        2, 1, false)},
    {"ER_EN", Combination(
        {LENGTH_EIGHTH, LENGTH_EIGHTH},
        {false, true},
        {{}, {}},
        2, 1, false)},
    {"EN_ER", Combination(
        {LENGTH_EIGHTH, LENGTH_EIGHTH},
        {true, false},
        {{}, {}},
        2, 1, false)},
    {"DEN_SN", Combination(
        {LENGTH_EIGHTH + LENGTH_16TH, LENGTH_16TH},
        {true, true},
        {{}, {}},
        2, 1, false)},
    {"SN_DEN", Combination(
        {LENGTH_16TH, LENGTH_EIGHTH + LENGTH_16TH},
        {true, true},
        {{}, {}},
        2, 1, false)},
    // 2 BEATS
    {"DQN_EN", Combination(
        {LENGTH_QUARTER + LENGTH_EIGHTH, LENGTH_EIGHTH},
        {true, true},
        {{}, {}},
        2, 2, false)},
    {"QR-ER_EN", Combination(
        {LENGTH_QUARTER + LENGTH_EIGHTH, LENGTH_EIGHTH},
        {false, true},
        {{}, {}},
        2, 2, false)},
    {"DQN_ER", Combination(
        {LENGTH_QUARTER + LENGTH_EIGHTH, LENGTH_EIGHTH},
        {true, false},
        {{}, {}},
        2, 2, false)},
    {"EN_EN-QN", Combination(
        {LENGTH_EIGHTH, LENGTH_QUARTER + LENGTH_EIGHTH},
        {true, true},
        {{}, {}},
        2, 2, false)},
    // 3 BEATS
    {"QN-DQN_EN", Combination(
        {LENGTH_HALF + LENGTH_EIGHTH, LENGTH_EIGHTH},
        {true, true},
        {{}, {}},
        2, 3, false)},
    {"QR-QR-ER_EN", Combination(
        {LENGTH_HALF + LENGTH_EIGHTH, LENGTH_EIGHTH},
        {false, true},
        {{}, {}},
        2, 3, false)},
    {"QN-DQN_ER", Combination(
        {LENGTH_HALF + LENGTH_EIGHTH, LENGTH_EIGHTH},
        {true, false},
        {{}, {}},
        2, 3, false)},
    {"EN_EN-HN", Combination(
        {LENGTH_EIGHTH, LENGTH_HALF + LENGTH_EIGHTH},
        {true, true},
        {{}, {}},
        2, 3, false)},
    // 4 BEATS
    {"HN-DQN_EN", Combination(
        {LENGTH_HALF + LENGTH_QUARTER + LENGTH_EIGHTH, LENGTH_EIGHTH},
        {true, true},
        {{}, {}},
        2, 4, false)},
    {"QR-QR-QR-ER_EN", Combination(
        {LENGTH_HALF + LENGTH_QUARTER + LENGTH_EIGHTH, LENGTH_EIGHTH},
        {false, true},
        {{}, {}},
        2, 4, false)},
    {"HN-DQN_ER", Combination(
        {LENGTH_HALF + LENGTH_QUARTER + LENGTH_EIGHTH, LENGTH_EIGHTH},
        {true, false},
        {{}, {}},
        2, 4, false)},
    {"EN_EN-DHN", Combination(
        {LENGTH_EIGHTH, LENGTH_HALF + LENGTH_QUARTER + LENGTH_EIGHTH},
        {true, true},
        {{}, {}},
        2, 4, false)},
    // 3 NOTES
    ////////////////////////////////////////////////////////////////////////////////////////////
    // 1 BEAT
    {"EN_SN_SN", Combination(
        {LENGTH_EIGHTH, LENGTH_16TH, LENGTH_16TH},
        {true, true, true},
        {  {std::make_pair(1, BeamOption::BEGIN)},
                    {std::make_pair(1, BeamOption::CONTINUE), std::make_pair(2, BeamOption::BEGIN)},
                    {std::make_pair(1, BeamOption::END), std::make_pair(2, BeamOption::END)}},
        3, 1, false)},
    {"ER_SN_SN", Combination(
        {LENGTH_EIGHTH, LENGTH_16TH, LENGTH_16TH},
        {false, true, true},
        {  {},
                    {std::make_pair(1, BeamOption::BEGIN), std::make_pair(2, BeamOption::BEGIN)},
                    {std::make_pair(1, BeamOption::END), std::make_pair(2, BeamOption::END)}},
        3, 1, false)},
    {"SN_SN_EN", Combination(
        {LENGTH_16TH, LENGTH_16TH, LENGTH_EIGHTH},
        {true, true, true},
        {  {std::make_pair(1, BeamOption::BEGIN), std::make_pair(2, BeamOption::BEGIN)},
                    {std::make_pair(1, BeamOption::CONTINUE), std::make_pair(2, BeamOption::END)},
                    {std::make_pair(1, BeamOption::END)}},
        3, 1, false)},
    {"SN_SN_ER", Combination(
        {LENGTH_16TH, LENGTH_16TH, LENGTH_EIGHTH},
        {true, true, false},
        {  {std::make_pair(1, BeamOption::BEGIN), std::make_pair(2, BeamOption::BEGIN)},
                    {std::make_pair(1, BeamOption::END), std::make_pair(2, BeamOption::END)},
                    {}},
        3, 1, false)},
    {"SN_EN_SN", Combination(
        {LENGTH_16TH, LENGTH_EIGHTH, LENGTH_16TH},
        {true, true, true},
        {  {std::make_pair(1, BeamOption::BEGIN)},
                    {std::make_pair(1, BeamOption::CONTINUE)},
                    {std::make_pair(1, BeamOption::END)}},
        3, 1, false)},
    {"T_EN_EN_EN", Combination(
        {LENGTH_T_EIGHTH, LENGTH_T_EIGHTH, LENGTH_T_EIGHTH},
        {true, true, true},
        {  {std::make_pair(1, BeamOption::BEGIN)},
                    {std::make_pair(1, BeamOption::CONTINUE)},
                    {std::make_pair(1, BeamOption::END)}},
        3, 1, true)},
    {"T_EN_DEN_SN", Combination(
        {LENGTH_T_EIGHTH, LENGTH_EIGHTH, LENGTH_T_16TH},
        {true, true, true},
        {  {std::make_pair(1, BeamOption::BEGIN)},
                    {std::make_pair(1, BeamOption::CONTINUE)},
                    {std::make_pair(1, BeamOption::END)}},
        3, 1, true)},
    {"T_EN_SN_DEN", Combination(
        {LENGTH_T_EIGHTH, LENGTH_T_16TH, LENGTH_EIGHTH},
        {true, true, true},
        {  {std::make_pair(1, BeamOption::BEGIN)},
                    {std::make_pair(1, BeamOption::CONTINUE)},
                    {std::make_pair(1, BeamOption::END)}},
        3, 1, true)},
    {"T_SN_EN_DEN", Combination(
        {LENGTH_T_16TH, LENGTH_T_EIGHTH, LENGTH_EIGHTH},
        {true, true, true},
        {  {std::make_pair(1, BeamOption::BEGIN)},
                    {std::make_pair(1, BeamOption::CONTINUE)},
                    {std::make_pair(1, BeamOption::END)}},
        3, 1, true)},
    {"T_SN_DEN_EN", Combination(
        {LENGTH_T_16TH, LENGTH_EIGHTH, LENGTH_T_EIGHTH},
        {true, true, true},
        {  {std::make_pair(1, BeamOption::BEGIN)},
                    {std::make_pair(1, BeamOption::CONTINUE)},
                    {std::make_pair(1, BeamOption::END)}},
        3, 1, true)},
    {"T_DEN_EN_SN", Combination(
        {LENGTH_EIGHTH, LENGTH_T_EIGHTH, LENGTH_T_16TH},
        {true, true, true},
        {  {std::make_pair(1, BeamOption::BEGIN)},
                    {std::make_pair(1, BeamOption::CONTINUE)},
                    {std::make_pair(1, BeamOption::END)}},
        3, 1, true)},
    {"T_DEN_SN_EN", Combination(
        {LENGTH_EIGHTH, LENGTH_T_16TH, LENGTH_T_EIGHTH},
        {true, true, true},
        {  {std::make_pair(1, BeamOption::BEGIN)},
                    {std::make_pair(1, BeamOption::CONTINUE)},
                    {std::make_pair(1, BeamOption::END)}},
        3, 1, true)},
    // 2 BEATS
    {"EN_QN_EN", Combination(
        {LENGTH_EIGHTH, LENGTH_QUARTER, LENGTH_EIGHTH},
        {true, true, true},
        {{}, {}, {}},
        3, 2, false)},
    {"DQN_SN_SN", Combination(
        {LENGTH_QUARTER + LENGTH_EIGHTH, LENGTH_16TH, LENGTH_16TH},
        {true, true, true},
        {  {},
                    {std::make_pair(1, BeamOption::BEGIN), std::make_pair(2, BeamOption::BEGIN)},
                    {std::make_pair(1, BeamOption::END), std::make_pair(2, BeamOption::END)}},
        3, 2, false)},
    // 3 BEATS
    {"QN-DQN_SN_SN", Combination(
        {LENGTH_HALF + LENGTH_EIGHTH, LENGTH_16TH, LENGTH_16TH},
        {true, true, true},
        {  {},
                    {std::make_pair(1, BeamOption::BEGIN), std::make_pair(2, BeamOption::BEGIN)},
                    {std::make_pair(1, BeamOption::END), std::make_pair(2, BeamOption::END)}},
        3, 3, false)},
    // 4 BEATS
    {"HN-DQN_SN_SN", Combination(
        {LENGTH_HALF + LENGTH_QUARTER + LENGTH_EIGHTH, LENGTH_16TH, LENGTH_16TH},
        {true, true, true},
        {  {},
                    {std::make_pair(1, BeamOption::BEGIN), std::make_pair(2, BeamOption::BEGIN)},
                    {std::make_pair(1, BeamOption::END), std::make_pair(2, BeamOption::END)}},
        3, 4, false)},
    // 4 NOTES
    ////////////////////////////////////////////////////////////////////////////////////////////
    // 1 BEAT
    {"SN_SN_SN_SN", Combination(
        {LENGTH_16TH, LENGTH_16TH, LENGTH_16TH, LENGTH_16TH},
        {true, true, true, true},
        {  {std::make_pair(1, BeamOption::BEGIN), std::make_pair(2, BeamOption::BEGIN)},
                    {std::make_pair(1, BeamOption::CONTINUE), std::make_pair(2, BeamOption::CONTINUE)},
                    {std::make_pair(1, BeamOption::CONTINUE), std::make_pair(2, BeamOption::CONTINUE)},
                    {std::make_pair(1, BeamOption::END), std::make_pair(2, BeamOption::END)}},
        4, 1, false)}
};

#endif /* COMBINATION */
