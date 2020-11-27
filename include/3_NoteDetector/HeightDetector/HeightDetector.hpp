#ifndef HEIGHT_DETECTOR
#define HEIGHT_DETECTOR

#include <vector>
#include "StepDetector.hpp"
#include "MusicXmlScore.hpp"


struct HeightOctave {
    int height;
    int octave;
};


struct NoteHeight {
    Step step;
    Alter alter;
    int octave;
};


NoteHeight getStepAlterOctave(int height, int octave, KeyFifths fifth);

std::tuple<Step, Alter> getMinorKey(KeyFifths fifth);

std::tuple<Step, Alter> getMajorKey(KeyFifths fifth);

bool isHeightInFifth(int height, KeyFifths fifth);


int medianValue(std::vector<int> data);


class HeightDetector {
public:
    // Constructors & Destructor
    HeightDetector();
    virtual ~HeightDetector();
    // Get-Set Attributes
    void setOffset(int offset);
    void setFifth(KeyFifths fifth);
    void setClef(ClefSign clef);
    void setMode(KeyMode mode);
    KeyFifths getFifth() const;
    ClefSign getClef() const;
    KeyMode getMode() const;
    // Other methods
    void setOptimumOffset(const std::vector<double> & pitchs_st);
    void findBestFifth(const std::vector<HeightOctave> & notes);
    int medianHeight(const std::vector<HeightOctave> & notes) const;
    void findBestClef(const std::vector<HeightOctave> & notes);
    void findMode(const std::vector<HeightOctave> & notes);
    std::vector<HeightOctave> getHeightAndOctave(const std::vector<double> & pitchs_st) const;
    HeightOctave getHeightAndOctave(double pitch_st) const;
    void fit(const StepResult & step_result);
    void fit(const std::vector<double> & pitchs_st);
    NoteHeight transform(double pitch_st) const;
    std::vector<NoteHeight> transform(const std::vector<double> & pitchs_st) const;
private:
    ClefSign clef;
    KeyFifths fifth;
    KeyMode mode;
    double offset;
};

#endif /* HEIGHT_DETECTOR */




