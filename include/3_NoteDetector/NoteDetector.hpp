#ifndef NOTE_DETECTOR
#define NOTE_DETECTOR

#include "StepDetector.hpp"
#include "HeightDetector.hpp"
#include "RhythmDetector.hpp"
#include <vector>
#include <map>
#include <string>
#include <atomic>

class NoteDetector {
public:
    // Constructors & Destructor
    NoteDetector(const StepResult & step_result);
    virtual ~NoteDetector();
    // Methods
    MusicXmlScore perform(  std::atomic<float> * progress,
                            RhythmParameters rhythm_parameters,
                            const std::string & title="Untitled",
                            const std::string & composer="Unknown",
                            const std::string & instrument="piano");
private:
    StepResult step_result;
    HeightDetector height_detector;
    RhythmDetector rhythm_detector;
};

#endif /* NOTE_DETECTOR */
