#include "RhythmDetector.hpp"
#include "StepDetector.hpp"
#include "Dijkstra.hpp"
#include <numeric>
#include <cmath>
#include <algorithm>
#include <map>
#include "Combinations.hpp"
#include <atomic>


CombinationFinder::CombinationFinder(const std::vector<std::string> & combinations_to_mask) {
    // Constructor
    this->combinations_masked = combinations_to_mask;
}


CombinationFinder::~CombinationFinder() {
    // Destructor
}


bool CombinationFinder::isMasked(const std::string & key_comb) const {
    return(std::find(this->combinations_masked.begin(), this->combinations_masked.end(), key_comb) != this->combinations_masked.end());
}


std::vector<double> CombinationFinder::calculateIdealRatios(const std::string & key_comb) const {
    std::vector<double> ratios;
    for(const auto & duration: COMBINATIONS.at(key_comb).durations) {
        ratios.push_back((double)duration / (double)(COMBINATIONS_DIVISION * COMBINATIONS.at(key_comb).nb_beats));
    }
    return ratios;
}


std::vector<double> CombinationFinder::calculateRealRatios(const std::vector<double> & lengths, const std::vector<double> & ideal_ratios/*={}*/) const {
    std::vector<double> real_ratios;
    double tot_length = 0.0;
    double tot_ratio = 1.0;
    for(unsigned int k = 0; k < lengths.size(); k++) {
        if(lengths[k] < 0) {
            tot_ratio -= ideal_ratios[k];
        } else {
            tot_length += lengths[k];
        }
    }
    for(unsigned int k = 0; k < lengths.size(); k++) {
        if(lengths[k] < 0) {
            real_ratios.push_back(ideal_ratios[k]);
        } else {
            real_ratios.push_back((lengths[k] / tot_length) * tot_ratio);
        }
    }
    return real_ratios;
}


double CombinationFinder::calculateTotalLength(const std::vector<double> & lengths, const std::vector<double> & ideal_ratios/*={}*/) const {
    double tot_length = 0.0;
    double tot_ratio = 1.0;
    for(unsigned int k = 0; k < lengths.size(); k++) {
        if(lengths[k] < 0) {
            tot_ratio -= ideal_ratios[k];
        } else {
            tot_length += lengths[k];
        }
    }
    return(tot_length / tot_ratio);
}


double CombinationFinder::calculateError(const std::string & key_comb, const std::vector<double> & lengths) const {
    std::vector<double> ideal_ratios = this->calculateIdealRatios(key_comb);
    std::vector<double> real_ratios = this->calculateRealRatios(lengths, ideal_ratios);
    double tot_length = this->calculateTotalLength(lengths, ideal_ratios);
    double error_length;
    double error_total = 0.0;
    for(unsigned int k = 0; k < ideal_ratios.size(); k++) {
        error_length = std::abs(ideal_ratios[k] - real_ratios[k]) * (double)COMBINATIONS.at(key_comb).nb_beats;
        error_total += sqrt(pow(ideal_ratios[k], 2.0) + pow(error_length, 2.0));
    }
    return(error_total * tot_length);
}


std::vector<double> CombinationFinder::getCorrectedLengths(const std::string & key_comb, const std::vector<double> & lengths) const {
    std::vector<double> ideal_ratios = this->calculateIdealRatios(key_comb);
    double tot_length = this->calculateTotalLength(lengths, ideal_ratios);
    std::vector<double> corrected_lengths;
    for(const auto & ratio: ideal_ratios) {
        corrected_lengths.push_back(ratio * tot_length);
    }
    return(corrected_lengths);
}


CombinationOptions CombinationFinder::findBestFit(const std::vector<double> & lengths, const std::vector<bool> & types, unsigned int nb_beats) const {
    CombinationOptions options;
    double error_temp;
    unsigned int nb_notes = lengths.size();
    for(const auto & comb: COMBINATIONS) {
        // If the combination is masked move to the next one
        if(this->isMasked(comb.first)) {
            continue;
        }
        // If the number of notes does not correspond
        if(COMBINATIONS.at(comb.first).nb_notes != nb_notes) {
            continue;
        }
        // If the number of beats does not correspond
        if(COMBINATIONS.at(comb.first).nb_beats != nb_beats) {
            continue;
        }
        // If the type of notes does not correspond
        if(COMBINATIONS.at(comb.first).types != types) {
            continue;
        }
        error_temp = this->calculateError(comb.first, lengths);
        options.insert({comb.first, error_temp});
    }
    return options;
}


ScoreRhythmBuilder::ScoreRhythmBuilder() {
    // Constructor
}


ScoreRhythmBuilder::~ScoreRhythmBuilder() {
    // Destructor
}


unsigned int ScoreRhythmBuilder::getBeatsRemainingInTheMeasure() const {
    return(this->beats_per_measure - (this->nb_beats_done % this->beats_per_measure));
}


unsigned int ScoreRhythmBuilder::getNumberOfMeasure() const {
    return(this->nb_beats_done / this->beats_per_measure);
}


Combination ScoreRhythmBuilder::adaptCombination(const std::string & key, unsigned int start_beat, unsigned int stop_beat) const {
    Combination comb = COMBINATIONS.at(key);
    unsigned int start_duration_to_ignore = start_beat * COMBINATIONS_DIVISION;
    for(const auto & duration: COMBINATIONS.at(key).durations) {
        if(duration == start_duration_to_ignore) {
            comb.durations.erase(comb.durations.begin());
            comb.types.erase(comb.types.begin());
            comb.beams.erase(comb.beams.begin());
            comb.nb_notes -= 1;
            start_duration_to_ignore = 0;
            break;
        } else if(duration < start_duration_to_ignore) {
            comb.durations.erase(comb.durations.begin());
            comb.types.erase(comb.types.begin());
            comb.beams.erase(comb.beams.begin());
            comb.nb_notes -= 1;
            start_duration_to_ignore -= duration;
        } else {
            comb.durations[0] -= start_duration_to_ignore;
            break;
        }
    }
    unsigned int end_duration_to_delete = std::accumulate(comb.durations.begin(), comb.durations.end(), 0);
    end_duration_to_delete -= ((stop_beat - start_beat) * COMBINATIONS_DIVISION);
    for(int ind = comb.durations.size() - 1; ind > -1; ind--) {
        if(comb.durations[ind] == end_duration_to_delete) {
            comb.durations.pop_back();
            comb.types.pop_back();
            comb.beams.pop_back();
            break;
        } else if(comb.durations[ind] < end_duration_to_delete) {
            end_duration_to_delete -= comb.durations[ind];
            comb.durations.pop_back();
            comb.types.pop_back();
            comb.beams.pop_back();
        } else {
            comb.durations[ind] -= end_duration_to_delete;
            break;
        }
    }
    return comb;
}


void ScoreRhythmBuilder::addNote(unsigned int duration, Beams beams, bool triplet, bool tie_start, bool tie_stop) {
    unsigned int duration_done = 0;
    while(duration_done < duration) {
        if(duration == (LENGTH_WHOLE + LENGTH_EIGHTH)) {
            // blanche pointée -> noire pointée :    4,5 beats
            this->notes.push_back(std::make_pair(this->note_index, NoteRhythm(LENGTH_HALF + LENGTH_QUARTER, Type::HALF, true, false, beams, {}, false, tie_stop)));
            this->notes.push_back(std::make_pair(this->note_index, NoteRhythm(LENGTH_QUARTER + LENGTH_EIGHTH, Type::QUARTER, true, false, beams, {}, false, false)));
            duration_done += (LENGTH_WHOLE + LENGTH_EIGHTH);
        } else if(duration >= LENGTH_WHOLE) {
            // ronde :                               4 beats
            this->notes.push_back(std::make_pair(this->note_index, NoteRhythm(LENGTH_WHOLE, Type::WHOLE, false, false, beams, {}, false, tie_stop)));
            duration_done += LENGTH_WHOLE;
        } else if(duration == (LENGTH_HALF + LENGTH_QUARTER + LENGTH_EIGHTH)) {
            // blanche -> noire pointée :            3,5 beats
            this->notes.push_back(std::make_pair(this->note_index, NoteRhythm(LENGTH_HALF, Type::QUARTER, false, false, beams, {}, false, tie_stop)));
            this->notes.push_back(std::make_pair(this->note_index, NoteRhythm(LENGTH_QUARTER + LENGTH_EIGHTH, Type::QUARTER, true, false, beams, {}, false, false)));
            duration_done += (LENGTH_HALF + LENGTH_QUARTER + LENGTH_EIGHTH);
        } else if(duration >= (LENGTH_HALF + LENGTH_QUARTER)) {
            // blanche pointée :                     3 beats
            this->notes.push_back(std::make_pair(this->note_index, NoteRhythm(LENGTH_HALF + LENGTH_QUARTER, Type::HALF, true, false, beams, {}, false, tie_stop)));
            duration_done += (LENGTH_HALF + LENGTH_QUARTER);
        } else if(duration == (LENGTH_HALF + LENGTH_EIGHTH)) {
            // noire -> noire pointée :              2,5 beats
            this->notes.push_back(std::make_pair(this->note_index, NoteRhythm(LENGTH_QUARTER, Type::QUARTER, false, false, beams, {}, false, tie_stop)));
            this->notes.push_back(std::make_pair(this->note_index, NoteRhythm(LENGTH_QUARTER + LENGTH_EIGHTH, Type::QUARTER, true, false, beams, {}, false, false)));
            duration_done += (LENGTH_HALF + LENGTH_EIGHTH);
        } else if(duration >= LENGTH_HALF) {
            // blanche :                             2 beats
            this->notes.push_back(std::make_pair(this->note_index, NoteRhythm(LENGTH_HALF, Type::HALF, false, false, beams, {}, false, tie_stop)));
            duration_done += LENGTH_HALF;
        } else if(duration >= (LENGTH_QUARTER + LENGTH_EIGHTH)) {
            // noire pointée :                       1,5 beats
            this->notes.push_back(std::make_pair(this->note_index, NoteRhythm(LENGTH_QUARTER + LENGTH_EIGHTH, Type::QUARTER, true, false, beams, {}, false, tie_stop)));
            duration_done += (LENGTH_QUARTER + LENGTH_EIGHTH);
        } else if(duration >= LENGTH_QUARTER) {
            // noire :                               1 beat
            this->notes.push_back(std::make_pair(this->note_index, NoteRhythm(LENGTH_QUARTER, Type::QUARTER, false, false, beams, {}, false, tie_stop)));
            duration_done += LENGTH_QUARTER;
        } else if(duration >= (LENGTH_EIGHTH + LENGTH_16TH)) {
            // croche pointée :                      0.75 beat
            this->notes.push_back(std::make_pair(this->note_index, NoteRhythm(LENGTH_EIGHTH + LENGTH_16TH, Type::EIGHTH, true, false, beams, {}, false, tie_stop)));
            duration_done += (LENGTH_EIGHTH + LENGTH_16TH);
        } else if(duration >= LENGTH_EIGHTH) {
            if(triplet) {
                // croche triolet pointée:              0.5 beat
                this->notes.push_back(std::make_pair(this->note_index, NoteRhythm(LENGTH_EIGHTH, Type::EIGHTH, true, true, beams, {}, false, tie_stop)));
            } else {
                // croche :                              0.5 beat
                this->notes.push_back(std::make_pair(this->note_index, NoteRhythm(LENGTH_EIGHTH, Type::EIGHTH, false, false, beams, {}, false, tie_stop)));
            }
            duration_done += LENGTH_EIGHTH;
        } else if(duration >= LENGTH_T_EIGHTH) {
            // croche triolet :                      0.33 beat
            this->notes.push_back(std::make_pair(this->note_index, NoteRhythm(LENGTH_T_EIGHTH, Type::EIGHTH, false, true, beams, {}, false, tie_stop)));
            duration_done += LENGTH_T_EIGHTH;
        } else if(duration >= LENGTH_16TH) {
            // double croche :                       0.25 beat
            this->notes.push_back(std::make_pair(this->note_index, NoteRhythm(LENGTH_16TH, Type::SIXTEENTH, false, false, beams, {}, false, tie_stop)));
            duration_done += LENGTH_16TH;
        } else if(duration >= LENGTH_T_16TH) {
            // double croche triolet :               0.1666 beat
            this->notes.push_back(std::make_pair(this->note_index, NoteRhythm(LENGTH_T_16TH, Type::SIXTEENTH, false, true, beams, {}, false, tie_stop)));
            duration_done += LENGTH_T_16TH;
        } else {
            throw std::runtime_error("Invalid input argument 'Lengths', the data value does not correspond to a known note rythm");
        }
        tie_stop = false;
    }
    if(duration_done != duration) {
        throw std::runtime_error("Error calculating the best note to write, wrong input values or not implemented rythm");
    }
    if(tie_start) {
        this->notes.back().second.tie_start = tie_start;
    }
}


// std::vector<Beams> ScoreRhythmBuilder::findBeams(const Combination & combination) const {
//     Beams temp_beam;
//     std::vector<Beams> beams;
//     for(const auto duration: combination.durations) {
//         beams.push_back(temp_beam);
//     }
//     if(combination.durations.size() == 4) {
//         if(combination.types[0] && combination.types[1] && combination.types[2] && combination.types[3]) {
//             if(combination.durations == COMBINATIONS.at("SN_SN_SN_SN").durations) {
//                 beams[0].insert(beams[0].begin(), std::make_pair(1, BeamOption::BEGIN));
//                 beams[0].insert(beams[0].begin(), std::make_pair(2, BeamOption::BEGIN));
//                 beams[1].insert(beams[1].begin(), std::make_pair(1, BeamOption::CONTINUE));
//                 beams[1].insert(beams[1].begin(), std::make_pair(2, BeamOption::CONTINUE));
//                 beams[2].insert(beams[2].begin(), std::make_pair(1, BeamOption::CONTINUE));
//                 beams[2].insert(beams[2].begin(), std::make_pair(2, BeamOption::CONTINUE));
//                 beams[3].insert(beams[3].begin(), std::make_pair(1, BeamOption::END));
//                 beams[3].insert(beams[3].begin(), std::make_pair(2, BeamOption::END));
//                 return beams;
//             }
//         }
//     }
//     if(combination.durations.size() == 3) {
//         if(combination.types[0] && combination.types[1] && combination.types[2]) {
//             if(combination.durations == COMBINATIONS.at("EN_SN_SN").durations) {
//                 beams[0].insert(beams[0].begin(), std::make_pair(1, BeamOption::BEGIN));
//                 beams[1].insert(beams[1].begin(), std::make_pair(1, BeamOption::CONTINUE));
//                 beams[1].insert(beams[1].begin(), std::make_pair(2, BeamOption::BEGIN));
//                 beams[2].insert(beams[2].begin(), std::make_pair(1, BeamOption::END));
//                 beams[2].insert(beams[2].begin(), std::make_pair(2, BeamOption::END));
//                 return beams;
//             }
//             if(combination.durations == COMBINATIONS.at("EN_SN_SN").durations) {
//                 beams[0].insert(beams[0].begin(), std::make_pair(1, BeamOption::BEGIN));
//                 beams[1].insert(beams[1].begin(), std::make_pair(1, BeamOption::CONTINUE));
//                 beams[1].insert(beams[1].begin(), std::make_pair(2, BeamOption::BEGIN));
//                 beams[2].insert(beams[2].begin(), std::make_pair(1, BeamOption::END));
//                 beams[2].insert(beams[2].begin(), std::make_pair(2, BeamOption::END));
//                 return beams;
//             }
//         }
//     }
// }


void ScoreRhythmBuilder::addCombinationPart(const std::string & key, unsigned int ind_beat_start, unsigned int ind_beat_stop) {
    Combination temp_comb = this->adaptCombination(key, ind_beat_start, ind_beat_stop);
    // std::vector<Beams> beams_per_note = this->findBeams(temp_comb);
    bool start_tied = false;
    bool end_tied = false;
    for(unsigned int k = 0; k < temp_comb.durations.size(); k++) {
        if((k == 0) && (ind_beat_start != 0) && (temp_comb.types[k])) {
            end_tied = true;
        }
        if((k == (temp_comb.durations.size() - 1)) && (ind_beat_stop != COMBINATIONS.at(key).nb_beats) && (temp_comb.types[k])) {
            start_tied = true;
        }
        this->addNote(temp_comb.durations[k], temp_comb.beams[k], temp_comb.triplet, start_tied, end_tied);
        if((k != (temp_comb.durations.size() - 1)) || (ind_beat_stop == COMBINATIONS.at(key).nb_beats)) {
            this->note_index += 1;
        }
    }
}


void ScoreRhythmBuilder::addCombination(const std::string & key) {
    unsigned int ind_beat_start = 0;
    while(ind_beat_start < COMBINATIONS.at(key).nb_beats) {
        unsigned int nb_beats_remaining_measure = this->getBeatsRemainingInTheMeasure();
        unsigned int nb_beats_remaining_combination = COMBINATIONS.at(key).nb_beats - ind_beat_start;
        unsigned int ind_beat_stop = ind_beat_start + std::min(nb_beats_remaining_measure, nb_beats_remaining_combination);
        this->addCombinationPart(key, ind_beat_start, ind_beat_stop);
        this->nb_beats_done += (ind_beat_stop - ind_beat_start);
        ind_beat_start = ind_beat_stop;
    }
}


void ScoreRhythmBuilder::completeLastMeasureWithRests() {
    unsigned int nb_beats_remaining = this->getBeatsRemainingInTheMeasure();
    for(unsigned int k = 0; k < nb_beats_remaining; k++) {
        this->notes.push_back(std::make_pair(-1, NoteRhythm(LENGTH_QUARTER, Type::QUARTER, false, false, {}, {}, false, false)));
    }
}


unsigned int ScoreRhythmBuilder::getMeasureNumber(size_t ind_note) const {
    unsigned int nb_beats = 0;
    for(size_t k = 0; k < ind_note; k++) {
        nb_beats += (unsigned int)this->notes[k].second.duration;
    }
    unsigned int no_measure = nb_beats / (this->beats_per_measure * LENGTH_QUARTER);
    return no_measure;
}


void ScoreRhythmBuilder::addBeamInformation() {
    size_t ind_note = 0;
    while(ind_note < this->notes.size()) {
        unsigned int current_no_measure = this->getMeasureNumber(ind_note);
        if((unsigned int)this->notes[ind_note].second.duration >= LENGTH_QUARTER) {
            ind_note += 1;
            continue;
        }
        if(ind_note + 3 < this->notes.size()) {
            if(this->getMeasureNumber(ind_note + 3) == current_no_measure) {
                if( (this->notes[ind_note].second.duration == LENGTH_16TH) && 
                    (this->notes[ind_note + 1].second.duration == LENGTH_16TH) && 
                    (this->notes[ind_note + 2].second.duration == LENGTH_16TH) && 
                    (this->notes[ind_note + 3].second.duration == LENGTH_16TH)) {
                    auto it = this->notes[ind_note].second.beams.begin();
                    this->notes[ind_note].second.beams.insert(it, std::make_pair(1, BeamOption::BEGIN));
                    this->notes[ind_note].second.beams.insert(it, std::make_pair(2, BeamOption::BEGIN));
                    it = this->notes[ind_note + 1].second.beams.begin();
                    this->notes[ind_note + 1].second.beams.insert(it, std::make_pair(1, BeamOption::CONTINUE));
                    this->notes[ind_note + 1].second.beams.insert(it, std::make_pair(2, BeamOption::CONTINUE));
                    it = this->notes[ind_note + 2].second.beams.begin();
                    this->notes[ind_note + 2].second.beams.insert(it, std::make_pair(1, BeamOption::CONTINUE));
                    this->notes[ind_note + 2].second.beams.insert(it, std::make_pair(2, BeamOption::CONTINUE));
                    it = this->notes[ind_note + 3].second.beams.begin();
                    this->notes[ind_note + 3].second.beams.insert(it, std::make_pair(1, BeamOption::END));
                    this->notes[ind_note + 3].second.beams.insert(it, std::make_pair(2, BeamOption::END));
                    ind_note += 4;
                    continue;
                }
            }
        }
        if(ind_note + 2 < this->notes.size()) {
            if(this->getMeasureNumber(ind_note + 2) == current_no_measure) {
                if( (this->notes[ind_note].second.duration == LENGTH_16TH) && 
                    (this->notes[ind_note + 1].second.duration == LENGTH_16TH) && 
                    (this->notes[ind_note + 2].second.duration == LENGTH_16TH)) {
                    auto it = this->notes[ind_note].second.beams.begin();
                    this->notes[ind_note].second.beams.insert(it, std::make_pair(1, BeamOption::BEGIN));
                    this->notes[ind_note].second.beams.insert(it, std::make_pair(2, BeamOption::BEGIN));
                    it = this->notes[ind_note + 1].second.beams.begin();
                    this->notes[ind_note + 1].second.beams.insert(it, std::make_pair(1, BeamOption::CONTINUE));
                    this->notes[ind_note + 1].second.beams.insert(it, std::make_pair(2, BeamOption::CONTINUE));
                    it = this->notes[ind_note + 2].second.beams.begin();
                    this->notes[ind_note + 2].second.beams.insert(it, std::make_pair(1, BeamOption::CONTINUE));
                    this->notes[ind_note + 2].second.beams.insert(it, std::make_pair(2, BeamOption::CONTINUE));
                    ind_note += 3;
                    continue;
                }
            }
        }
        // if(ind_note + 1 < this->notes.size()) {
        //     if(this->getMeasureNumber(ind_note + 1) == current_no_measure) {

        //     }
        // }
        ind_note += 1;
    }
}


RhythmResult ScoreRhythmBuilder::perform(const std::vector<std::string> & combinations, unsigned int beats_per_measure, int offset_index/*=0*/) {
    this->notes.clear();
    this->note_index = offset_index;
    this->nb_beats_done = 0;
    this->beats_per_measure = beats_per_measure;
    for(const auto & key : combinations) {
        this->addCombination(key);
    }
    this->completeLastMeasureWithRests();
    this->addBeamInformation();
    return this->notes;
}


Configuration::Configuration(   const notepath & path,
                                unsigned int nb_beats,
                                const CombinationOptions & options,
                                const std::vector<double> & note_lengths_s) {
    // Constructor
    this->path = path;
    this->nb_beats = nb_beats;
    this->options = options;
    this->note_lengths_s = note_lengths_s;
}


Configuration::~Configuration() {
    // Destructor
}


bool Configuration::isValid(double delay_min_s, double delay_max_s, double error_max, const std::vector<std::string> & combinations_masked) const {
    for(const auto & option: this->options) {
        bool masked = std::find(combinations_masked.begin(), combinations_masked.end(), option.first) != combinations_masked.end();
        double delay_s = this->getDelay(option.first);
        if((option.second < error_max) && !masked && (delay_s <= delay_max_s) && (delay_s >= delay_min_s)) {
            return true;
        }
    }
    return false;
}


std::string Configuration::getBestCombination(const std::vector<std::string> & combinations_masked) const {
    std::string best_combination;
    double best_error = std::numeric_limits<double>::max();
    for(const auto & option: this->options) {
        bool masked = std::find(combinations_masked.begin(), combinations_masked.end(), option.first) != combinations_masked.end();
        if(masked) {
            continue;
        }
        if(option.second < best_error) {
            best_error = option.second;
            best_combination = option.first;
        }
    }
    return best_combination;
}


notepath Configuration::getPath() const {
    return this->path;
}


unsigned int Configuration::getNbNotes() const {
    return (unsigned int)this->note_lengths_s.size();
}


unsigned int Configuration::getNbBeats() const {
    return this->nb_beats;
}


std::vector<double> Configuration::getLengths(const std::string key_comb) const {
    CombinationFinder comb_finder;
    std::vector<double> note_lengths_corrected = comb_finder.getCorrectedLengths(key_comb, this->note_lengths_s);
    return note_lengths_corrected;
}


double Configuration::getTotalLength(const std::string key_comb) const {
    std::vector<double> lengths_s = this->getLengths(key_comb);
    double length_tot_s = std::accumulate(lengths_s.begin(), lengths_s.end(), 0.0);
    return length_tot_s;
}


double Configuration::getDelay(const std::string key_comb) const {
    double length_s = this->getTotalLength(key_comb);
    return length_s / (double)nb_beats;
}


double Configuration::getError(const std::string key_comb) const {
    return this->options.at(key_comb);
}


RhythmDetector::RhythmDetector() {
    // Constructor
    this->graph = Dijkstra();
    this->divisions = (int)COMBINATIONS_DIVISION;
}


RhythmDetector::~RhythmDetector() {
    // Destructor
}


std::vector<notepath> RhythmDetector::getPaths(int64_t index, unsigned int nb_notes) const {
    std::vector<notepath> vec_of_paths;
    std::vector<notepath> vec_of_paths_temp;
    vec_of_paths.push_back({{(int)index}});
    if((index + 1 < (int64_t)this->step_result.notes.size()) && (index >= 0)) {
        if(this->step_result.notes[index].is_a_note && !this->step_result.notes[index + 1].is_a_note) {
            vec_of_paths.push_back({{(int)index, (int)index + 1}});
        }
    }
    bool modification = true;
    while(modification) {
        modification = false;
        vec_of_paths_temp.clear();
        // Iterate over all current paths
        for(size_t k_path = 0; k_path < vec_of_paths.size(); k_path++) {
            notepath path = vec_of_paths[k_path];
            if(path.size() < nb_notes) {
                // If the number of note of the path did not reach the number of note desired
                int next_index = path.back().back() + 1;
                // Add the next note to the current path if it does not exceed the total number of notes
                if(next_index < (int64_t)this->step_result.notes.size()) {
                    path.push_back({next_index});
                    vec_of_paths_temp.push_back(path);
                    modification = true;
                }
                // If the next note is followed by a rest and does not exceed the total number of notes
                // adding both the next note and the rest to be considered as a single note
                if(next_index + 1 < (int64_t)this->step_result.notes.size()) {
                    if(this->step_result.notes[next_index].is_a_note && !this->step_result.notes[next_index + 1].is_a_note) {
                        path = vec_of_paths[k_path];
                        path.push_back({next_index, next_index + 1});
                        vec_of_paths_temp.push_back(path);
                    }
                }
            } else {
                // If the number of note of the path already reached the number of note desired
                vec_of_paths_temp.push_back(path);
            }
        }
        vec_of_paths = vec_of_paths_temp;
    }
    return vec_of_paths;
}


std::vector<double> RhythmDetector::getNoteLengths(const notepath & path) const {
    std::vector<double> note_lengths;
    for(const auto & indexes: path) {
        if(indexes.size() == 1) {
            if(indexes[0] < 0) {
                if(path.size() < 2) {
                    throw std::runtime_error("Simulated rest detected, but the size of the notes path is only 1"); 
                }
                // Setting the length eqaul to -1 <=> the optimal length will be used for calculating the optimal combination
                note_lengths.push_back(-1.0);
            } else {
                note_lengths.push_back(this->step_result.notes[indexes[0]].length_s);
            }
        } else {
            double length_s = 0.0;
            for(auto index: indexes) {
                length_s += this->step_result.notes[index].length_s;
            }
            note_lengths.push_back(length_s);
        }
    }
    return note_lengths;
}


std::vector<bool> RhythmDetector::getNoteTypes(const notepath & path) const {
    std::vector<bool> note_types;
    for(const auto & indexes: path) {
        if(indexes.size() == 1) {
            if(indexes[0] < 0) {
                note_types.push_back(false);
            } else {
                note_types.push_back(this->step_result.notes[indexes[0]].is_a_note);
            }
        } else {
            bool type = false;
            for(auto index: indexes) {
                if(this->step_result.notes[index].is_a_note) {
                    type = true;
                    break;
                }
            }
            note_types.push_back(type);
        }
    }
    return note_types;
}


void RhythmDetector::getConfigurations() {
    CombinationFinder comb_finder;
    this->configurations.clear();
    unsigned int nb_conf = 0;
    std::string conf_ref;
    for(int64_t ind_note = -1; ind_note < (int64_t)this->step_result.notes.size(); ind_note++) {
        // If we are testing the simulated rest [ind_note == -1]
        // So we don't want to test the cases where the rest is only an integer number of beats
        unsigned int nb_notes_start = 1;
        if(ind_note < 0) {
            nb_notes_start = 2;
        }
        for(unsigned int nb_notes = nb_notes_start; nb_notes < 5; nb_notes++) {
            // Get paths: indexes of 'nb_notes' next notes
            std::vector<notepath> paths = this->getPaths(ind_note, nb_notes);
            for(const auto & path: paths) {
                std::vector<double> note_lengths = this->getNoteLengths(path);
                std::vector<bool> note_types = this->getNoteTypes(path);
                for(unsigned int nb_beats = 1; nb_beats < 9; nb_beats++) {
                    CombinationOptions options = comb_finder.findBestFit(note_lengths, note_types, nb_beats);
                    if(options.empty()) {
                        continue;
                    }
                    Configuration configuration(path, nb_beats, options, note_lengths);
                    // Add the configuration to the list
                    conf_ref = "CONF_" + std::to_string(nb_conf);
                    this->configurations.insert(std::pair<std::string, Configuration>(conf_ref, configuration));
                    nb_conf += 1;
                }
            }
        }
    }
}


bool RhythmDetector::isConfigurationValid(const std::string & config_key) const {
    return this->configurations.at(config_key).isValid(this->delay_min_s, this->delay_max_s, this->error_max, this->combinations_masked);
}


std::vector<std::string> RhythmDetector::getConfigurationsStartingWithIndex(int index) const {
    std::vector<std::string> configs_res;
    for(const auto & config: this->configurations) {
        if(!this->isConfigurationValid(config.first)) {
            continue;
        }
        notepath path = config.second.getPath();
        if(path[0][0] == index) {
            configs_res.push_back(config.first);
        }
    }
    return configs_res;
}


double RhythmDetector::getDelayWeight(double current_delay_s, double next_delay_s, double length_s) const {
    return(sqrt(pow(10.0 * log10(next_delay_s) - 10.0 * log10(current_delay_s), 2.0) + pow(length_s, 2.0)));
}


void RhythmDetector::buildGraph() {
    double weight_adjust = 0.5;
    std::vector<std::string> config_keys;
    // Clearing the graph
    this->graph.clear();
    // Adding all the vertex
    this->graph.add_vertex("START");
    this->graph.add_vertex("END");
    for(const auto & config: this->configurations) {
        if(this->isConfigurationValid(config.first)) {
            this->graph.add_vertex(config.first);
        }
    }
    // Creating edges from 'START' vertex to simulated rest at the beginning
    config_keys = this->getConfigurationsStartingWithIndex(-1);
    for(auto key: config_keys) {
        std::string best_comb_key = this->configurations.at(key).getBestCombination(this->combinations_masked);
        double rest_length_s = this->configurations.at(key).getLengths(best_comb_key)[0];
        this->graph.add_edge("START", key, -rest_length_s);
    }
    // Creating edges from 'START' vertex to the first note
    config_keys = this->getConfigurationsStartingWithIndex(0);
    for(auto key: config_keys) {
        this->graph.add_edge("START", key, 0.0);
    }
    // Creating edges between all configurations
    for(const auto & config: this->configurations) {
        std::string current_key = config.first;
        if(!this->isConfigurationValid(current_key)) {
            continue;
        }
        std::string best_comb_key = this->configurations.at(current_key).getBestCombination(this->combinations_masked);
        double current_delay_s = config.second.getDelay(best_comb_key);
        double current_comberror = config.second.getError(best_comb_key);
        int64_t next_index = config.second.getPath().back().back() + 1;
        double length_s = this->configurations.at(current_key).getTotalLength(best_comb_key);
        if(next_index >= (int64_t)this->step_result.notes.size()) {
            // The configuration reached the end
            this->graph.add_edge(current_key, "END", length_s);
        } else {
            // Get configurations corresponding to the next note
            config_keys = this->getConfigurationsStartingWithIndex(next_index);
            for(auto next_key: config_keys) {
                std::string best_comb_key_next = this->configurations.at(next_key).getBestCombination(this->combinations_masked);
                double next_delay_s = this->configurations.at(next_key).getDelay(best_comb_key_next);
                // If the difference of delay between the 2 configurations is too high
                if((abs(current_delay_s - next_delay_s) / current_delay_s) > this->max_delay_var) {
                    continue;
                }
                double delay_weight = this->getDelayWeight(current_delay_s, next_delay_s, length_s);
                double comberror_weight = current_comberror;
                double weight = (1.0 - weight_adjust) * delay_weight + weight_adjust * comberror_weight;
                this->graph.add_edge(current_key, next_key, weight);
            }
        }
    }
}


int RhythmDetector::getOptimalPath() {
    this->best_path = this->graph.shortest_path("START", "END");
    if(this->best_path.empty()) {
        return -1;
    }
    std::reverse(this->best_path.begin(), this->best_path.end());
    this->best_path.pop_back();
    return 0;
}


BeatInfo RhythmDetector::getPrettiestScoreParameters() {
    std::vector<unsigned int> list_nb_beats;
    std::vector<std::string> list_combs;
    for(auto key_config: this->best_path) {
        list_nb_beats.push_back(this->configurations.at(key_config).getNbBeats());
        list_combs.push_back(this->configurations.at(key_config).getBestCombination(this->combinations_masked));
    }
    std::vector<BeatInfo> parameters_tested;
    // Testing measure time beat only 3/4, 4/4 and 5/4
    for(unsigned int beats_per_measure = 3; beats_per_measure < 6 ; beats_per_measure++) {
        // Testing to add rests at the beginning to minimize the number of combinations cut
        for(unsigned int nb_rests_added = 0; nb_rests_added < beats_per_measure; nb_rests_added++) {
            std::vector<unsigned int> list_nb_beats_tested = list_nb_beats;
            list_nb_beats_tested.insert(list_nb_beats_tested.begin(), nb_rests_added, 1);
            std::vector<unsigned int> cumsum_nb_beats(list_nb_beats_tested.size());
            std::partial_sum(list_nb_beats_tested.begin(), list_nb_beats_tested.end(), cumsum_nb_beats.begin());
            std::vector<std::string> combinations_cut;
            unsigned int measure_number = 0;
            for(unsigned int k = 0; k < cumsum_nb_beats.size(); k++) {
                unsigned int nb_measure_temp = cumsum_nb_beats[k] / beats_per_measure;
                if(nb_measure_temp > measure_number) {
                    if((cumsum_nb_beats[k] % beats_per_measure) != 0) {
                        combinations_cut.push_back(list_combs[k - nb_rests_added]);
                    }
                    measure_number = nb_measure_temp;
                }
            }
            // Adding it the the list of parameters tested for later comparison
            parameters_tested.push_back({beats_per_measure, nb_rests_added, combinations_cut});
        }
    }
    BeatInfo best_param = parameters_tested[0];
    for(const auto & parameter: parameters_tested) {
        if(parameter.combinations_cut.size() < best_param.combinations_cut.size()) {
            best_param = parameter;
        }
    }
    this->beats_per_measure = best_param.beats_per_measure;
    this->beattype = 4;
    return best_param;
}


std::vector<std::string> RhythmDetector::getBestpathCombinations() const {
    std::vector<std::string> path;
    for(const auto & config_key: this->best_path) {
        std::string comb_key = this->configurations.at(config_key).getBestCombination(this->combinations_masked);
        if(comb_key.empty()) {
            throw std::runtime_error("Best combination not found ! this error should never happen."); 
        }
        path.push_back(comb_key);
    }
    return path;
}


void RhythmDetector::addRests(unsigned int nb_rests, std::vector<std::string> & combinations_key) const {
    // std::vector<std::string> rests(nb_rests, "1REST_1BEAT");
    combinations_key.insert(combinations_key.begin(), nb_rests, "1REST_1BEAT");
}


void RhythmDetector::fit(const StepResult & step_result) {
    this->step_result = step_result;
    this->graph.clear();
    this->best_path.clear();
    this->getConfigurations();
}


int RhythmDetector::getBeats() const {
    return this->beats_per_measure;
}


int RhythmDetector::getBeatType() const {
    return this->beattype;
}


int RhythmDetector::getDivisions() const {
    return this->divisions;
}


RhythmResult RhythmDetector::perform(std::atomic<float> * progress, const RhythmParameters & parameters/*=DEFAULT_RHYTHM_PARAMETERS*/) {
    // Setting parameters
    this->delay_max_s = parameters.delay_max_s;
    this->delay_min_s = parameters.delay_min_s;
    this->error_max = parameters.error_max;
    this->max_delay_var = parameters.max_delay_var;
    this->combinations_masked = parameters.combinations_masked;
    *progress = 0.0;
    if(progress->load() < 0) {
        throw std::runtime_error("Process cancel by the user");
    }
    this->buildGraph();
    if(progress->load() < 0) {
        throw std::runtime_error("Process cancel by the user");
    }
    *progress = 20.0;
    if(this->getOptimalPath() < 0) {
        throw std::runtime_error("Path not found"); 
    }
    if(progress->load() < 0) {
        throw std::runtime_error("Process cancel by the user");
    }
    *progress = 40.0;
    std::vector<std::string> comb_path = this->getBestpathCombinations();
    BeatInfo params = this->getPrettiestScoreParameters();
    this->addRests(params.nb_rests, comb_path);
    if(progress->load() < 0) {
        throw std::runtime_error("Process cancel by the user");
    }
    *progress = 60.0;
    ScoreRhythmBuilder builder;
    RhythmResult result = builder.perform(comb_path, params.beats_per_measure, -params.nb_rests);
    return result;
}
