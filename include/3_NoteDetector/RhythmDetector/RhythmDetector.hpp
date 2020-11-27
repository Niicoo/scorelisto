#ifndef RHYTHM_DETECTOR
#define RHYTHM_DETECTOR

#include "StepDetector.hpp"
#include "Combinations.hpp"
#include "Dijkstra.hpp"
#include <vector>
#include <atomic>

///////////////////////////////////////////////////
///////////////////////////////////////////////////
typedef std::map<std::string, double> CombinationOptions;

class CombinationFinder {
public:
    // Constructors & Destructor
    CombinationFinder(const std::vector<std::string> & combinations_to_mask={});
    virtual ~CombinationFinder();
    // Methods
    bool isMasked(const std::string & key_comb) const;
    std::vector<double> calculateIdealRatios(const std::string & key_comb) const;
    std::vector<double> calculateRealRatios(const std::vector<double> & lengths, const std::vector<double> & ideal_ratios={}) const;
    double calculateTotalLength(const std::vector<double> & lengths, const std::vector<double> & ideal_ratios={}) const;
    double calculateError(const std::string & key_comb, const std::vector<double> & lengths) const;
    std::vector<double> getCorrectedLengths(const std::string & key_comb, const std::vector<double> & lengths) const;
    CombinationOptions findBestFit(const std::vector<double> & lengths, const std::vector<bool> & types, unsigned int nb_beats) const;
private:
    std::vector<std::string> combinations_masked;
};
///////////////////////////////////////////////////
///////////////////////////////////////////////////


///////////////////////////////////////////////////
///////////////////////////////////////////////////
struct NoteRhythm {
    NoteRhythm(int duration, Type type, bool dot, bool triplet, Beams beams, Slurs slurs, bool tie_start, bool tie_stop) {
        this->duration = duration;
        this->type = type;
        this->dot = dot;
        this->triplet = triplet;
        this->beams = beams;
        this->slurs = slurs;
        this->tie_start = tie_start;
        this->tie_stop = tie_stop;
    }
    int duration;
    Type type;
    bool dot;
    bool triplet;
    Beams beams;
    Slurs slurs;
    bool tie_start;
    bool tie_stop;
};
typedef std::vector<std::pair<int, NoteRhythm>> RhythmResult;


class ScoreRhythmBuilder {
public:
    // Constructors & Destructor
    ScoreRhythmBuilder();
    virtual ~ScoreRhythmBuilder();
    // Methods
    unsigned int getBeatsRemainingInTheMeasure() const;
    unsigned int getNumberOfMeasure() const;
    std::vector<Beams> findBeams(const Combination & combination) const;
    Combination adaptCombination(const std::string & key, unsigned int start_beat, unsigned int stop_beat) const;
    void addNote(unsigned int duration, Beams beams, bool triplet, bool tie_start, bool tie_stop);
    void addCombinationPart(const std::string & key, unsigned int ind_beat_start, unsigned int ind_beat_stop);
    void addCombination(const std::string & key);
    void completeLastMeasureWithRests();
    unsigned int getMeasureNumber(size_t ind_note) const;
    void addBeamInformation();
    RhythmResult perform(const std::vector<std::string> & combinations, unsigned int beats_per_measure, int offset_index=0);
private:
    RhythmResult notes;
    int note_index;
    unsigned int nb_beats_done;
    unsigned int beats_per_measure;
};
///////////////////////////////////////////////////
///////////////////////////////////////////////////


///////////////////////////////////////////////////
///////////////////////////////////////////////////
typedef std::vector<std::vector<int64_t>> notepath;


class Configuration {
public:
    // Constructors & Destructor
    Configuration(  const notepath & path,
                    unsigned int nb_beats,
                    const CombinationOptions & options,
                    const std::vector<double> & note_lengths_s);
    virtual ~Configuration();
    // Methods
    bool isValid(double delay_min_s, double delay_max_s, double error_max, const std::vector<std::string> & combinations_masked) const;
    std::string getBestCombination(const std::vector<std::string> & combinations_masked) const;
    std::vector<double> getLengths(const std::string key_comb) const;
    double getTotalLength(const std::string key_comb) const;
    double getDelay(const std::string key_comb) const;
    double getError(const std::string key_comb) const;
    notepath getPath() const;
    unsigned int getNbNotes() const;
    unsigned int getNbBeats() const;
private:
    // Attributes
    notepath path;
    unsigned int nb_beats;
    CombinationOptions options;
    std::vector<double> note_lengths_s;
};
///////////////////////////////////////////////////
///////////////////////////////////////////////////


///////////////////////////////////////////////////
///////////////////////////////////////////////////
struct BeatInfo {
    unsigned int beats_per_measure;
    unsigned int nb_rests;
    std::vector<std::string> combinations_cut;
};


struct RhythmParameters {
    double delay_min_s;
    double delay_max_s;
    double error_max;
    double max_delay_var;
    std::vector<std::string> combinations_masked;
};
const RhythmParameters DEFAULT_RHYTHM_PARAMETERS = {0.3, 1.5, 10.0, 0.5, {}};


class RhythmDetector {
public:
    // Constructors & Destructor
    RhythmDetector();
    virtual ~RhythmDetector();
    // Methods
    void fit(const StepResult & step_result);
    RhythmResult perform(std::atomic<float> * progress, const RhythmParameters & parameters=DEFAULT_RHYTHM_PARAMETERS);
    int getBeats() const;
    int getBeatType() const;
    int getDivisions() const;
private:
    // Attributes
    StepResult step_result;
    int beats_per_measure;
    int beattype;
    int divisions;
    std::map<std::string, Configuration> configurations;
    Dijkstra graph;
    std::vector<std::string> best_path;
    double delay_max_s;
    double delay_min_s;
    double error_max;
    double max_delay_var;
    std::vector<std::string> combinations_masked;
    // Functions
    std::vector<notepath> getPaths(int64_t index, unsigned int nb_notes) const;
    std::vector<double> getNoteLengths(const notepath & path) const;
    std::vector<bool> getNoteTypes(const notepath & path) const;
    void getConfigurations();
    bool isConfigurationValid(const std::string & config_key) const;
    std::vector<std::string> getConfigurationsStartingWithIndex(int index) const;
    double getDelayWeight(double current_delay_s, double next_delay_s, double length_s) const;
    void buildGraph();
    int getOptimalPath();
    BeatInfo getPrettiestScoreParameters();
    std::vector<std::string> getBestpathCombinations() const;
    void addRests(unsigned int nb_rests, std::vector<std::string> & combinations_key) const;
};
///////////////////////////////////////////////////
///////////////////////////////////////////////////


#endif /* RHYTHM_DETECTOR */
