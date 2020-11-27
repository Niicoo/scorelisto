#include "NoteDetector.hpp"
#include "HeightDetector.hpp"
#include "RhythmDetector.hpp"
#include <atomic>


NoteDetector::NoteDetector(const StepResult & step_result) {
    // Constructor
    this->step_result = step_result;
    this->height_detector = HeightDetector();
    this->height_detector.fit(this->step_result);
    this->rhythm_detector = RhythmDetector();
    this->rhythm_detector.fit(this->step_result);
}


NoteDetector::~NoteDetector() {
    // Destructor
}


MusicXmlScore NoteDetector::perform(std::atomic<float> * progress,
                                    RhythmParameters rhythm_parameters,
                                    const std::string & title/*="Untitled"*/,
                                    const std::string & composer/*="Unknown"*/,
                                    const std::string & instrument/*="piano"*/) {
    RhythmResult noterhythms = this->rhythm_detector.perform(progress, rhythm_parameters);
    if(progress->load() < 0) {
        throw std::runtime_error("Process cancel by the user");
    }
    *progress = 99.0;
    // Init the score parameters
    MusicXmlScore score(title, composer);
    std::string PartId = "P1";
    score.addPart(PartId, instrument);
    // Create the attributes
    AttributesElement* attributes = new AttributesElement();
    attributes->setDivisions(this->rhythm_detector.getDivisions());
    attributes->setFifths(this->height_detector.getFifth());
    attributes->setTime(this->rhythm_detector.getBeats(), this->rhythm_detector.getBeatType());
    attributes->setClef(this->height_detector.getClef());
    score.pushAttributes(attributes, PartId);
    for(const auto & note: noterhythms) {
        if(progress->load() < 0) {
            throw std::runtime_error("Process cancel by the user");
        }
        bool is_a_note;
        if(note.first < 0) {
            is_a_note = false;
        } else {
            is_a_note = this->step_result.notes[note.first].is_a_note;
        }
        NoteElement* note_el = new NoteElement(is_a_note);
        if(is_a_note) {
            double pitch_st = this->step_result.notes[note.first].pitch_st;
            NoteHeight height = this->height_detector.transform(pitch_st);
            note_el->setPitch(height.step, height.octave, height.alter);
        }
        note_el->setDuration(note.second.duration);
        note_el->setType(note.second.type);
        note_el->setDot(note.second.dot);
        note_el->setBeams(note.second.beams);
        note_el->setSlurs(note.second.slurs);
        score.pushNote(note_el, PartId);
    }
    *progress = 100.0;
    return score;
}
