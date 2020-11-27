#include "HeightDetector.hpp"
#include "StepDetector.hpp"
#include <climits>
#include <cmath>
#include <algorithm>

NoteHeight getStepAlterOctave(int height, int octave, KeyFifths fifth) {
    NoteHeight result;
    if(height == 0) {
      if(int(fifth) >= 7) {
        // SI DIESE
        result.step = Step::B;
        result.alter = Alter::SHARP;
        result.octave = octave - 1;
      } else {
        // DO
        result.step = Step::C;
        result.alter = Alter::NATURAL;
        result.octave = octave;
      }

    } else if(height == 1) {
      if(int(fifth) >= -3) {
        // DO DIESE
        result.step = Step::C;
        result.alter = Alter::SHARP;
        result.octave = octave;
      } else {
        // RE BEMOL
        result.step = Step::D;
        result.alter = Alter::FLAT;
        result.octave = octave;
      }
    
    } else if(height == 2) {
        // RE
        result.step = Step::D;
        result.alter = Alter::NATURAL;
        result.octave = octave;

    } else if(height == 3) {
      if(int(fifth) >= 4) {
        // RE DIESE
        result.step = Step::D;
        result.alter = Alter::SHARP;
        result.octave = octave;
      } else {
        // MI BEMOL
        result.step = Step::E;
        result.alter = Alter::FLAT;
        result.octave = octave;
      }
    
    } else if(height == 4) {
      if(int(fifth) <= -7) {
        // FA BEMOL
        result.step = Step::F;
        result.alter = Alter::FLAT;
        result.octave = octave;
      } else {
        // MI
        result.step = Step::E;
        result.alter = Alter::NATURAL;
        result.octave = octave;
      }
    
    } else if(height == 5) {
      if(int(fifth) >= 6) {
        // MI DIESE
        result.step = Step::E;
        result.alter = Alter::SHARP;
        result.octave = octave;
      } else {
        // FA
        result.step = Step::F;
        result.alter = Alter::NATURAL;
        result.octave = octave;
      }
    
    } else if(height == 6) {
      if(int(fifth) >= -4) {
        // FA DIESE
        result.step = Step::F;
        result.alter = Alter::SHARP;
        result.octave = octave;
      } else {
        // SOL BEMOL
        result.step = Step::G;
        result.alter = Alter::FLAT;
        result.octave = octave;
      }
    
    } else if(height == 7) {
        // SOL
        result.step = Step::G;
        result.alter = Alter::NATURAL;
        result.octave = octave;

    } else if(height == 8) {
      if(int(fifth) > 0) {
        // SOL DIESE
        result.step = Step::G;
        result.alter = Alter::SHARP;
        result.octave = octave;
      } else {
        // LA BEMOL
        result.step = Step::A;
        result.alter = Alter::FLAT;
        result.octave = octave;
      }
    
    } else if(height == 9) {
        // LA
        result.step = Step::A;
        result.alter = Alter::NATURAL;
        result.octave = octave;

    } else if(height == 10) {
      if(int(fifth) >= 5) {
        // LA DIESE
        result.step = Step::A;
        result.alter = Alter::SHARP;
        result.octave = octave;
      } else {
        // SI BEMOL
        result.step = Step::B;
        result.alter = Alter::FLAT;
        result.octave = octave;
      }
    
    } else if(height == 11) {
      if(int(fifth) >= -5) {
        // SI
        result.step = Step::B;
        result.alter = Alter::NATURAL;
        result.octave = octave;
      } else {
        // DO BEMOL
        result.step = Step::C;
        result.alter = Alter::FLAT;
        result.octave = octave + 1;
      }
    } else {
        throw std::runtime_error("Something wrong happened, invalid fifth or invalid estimated height");
    }
    return result;
}


std::tuple<Step, Alter> getMinorKey(KeyFifths fifth) {
    switch(fifth){
        case KeyFifths::ZERO:
            // LA
            return std::make_tuple(Step::A, Alter::NATURAL);
        case KeyFifths::ONE_SHARP:
            // MI
            return std::make_tuple(Step::E, Alter::NATURAL);
        case KeyFifths::TWO_SHARPS:
            // SI
            return std::make_tuple(Step::B, Alter::NATURAL);
        case KeyFifths::THREE_SHARPS:
            // FA SHARP
            return std::make_tuple(Step::F, Alter::SHARP);
        case KeyFifths::FOUR_SHARPS:
            // DO SHARP
            return std::make_tuple(Step::C, Alter::SHARP);
        case KeyFifths::FIVE_SHARPS:
            // SOL SHARP
            return std::make_tuple(Step::G, Alter::SHARP);
        case KeyFifths::SIX_SHARPS:
            // RE SHARP
            return std::make_tuple(Step::D, Alter::SHARP);
        case KeyFifths::SEVEN_SHARPS:
            // LA SHARP
            return std::make_tuple(Step::A, Alter::SHARP);
        case KeyFifths::ONE_FLAT:
            // RE
            return std::make_tuple(Step::D, Alter::NATURAL);
        case KeyFifths::TWO_FLATS:
            // SOL
            return std::make_tuple(Step::G, Alter::NATURAL);
        case KeyFifths::THREE_FLATS:
            // DO
            return std::make_tuple(Step::C, Alter::NATURAL);
        case KeyFifths::FOUR_FLATS:
            // FA
            return std::make_tuple(Step::F, Alter::NATURAL);
        case KeyFifths::FIVE_FLATS:
            // SI FLAT
            return std::make_tuple(Step::B, Alter::FLAT);
        case KeyFifths::SIX_FLATS:
            // MI FLAT
            return std::make_tuple(Step::E, Alter::FLAT);
        case KeyFifths::SEVEN_FLATS:
            // LA FLAT
            return std::make_tuple(Step::A, Alter::FLAT);
        default:
            throw std::runtime_error("Input fifth option does not exists");
    }
}


std::tuple<Step, Alter> getMajorKey(KeyFifths fifth) {
    switch(fifth){
        case KeyFifths::ZERO:
            // DO
            return std::make_tuple(Step::C, Alter::NATURAL);
        case KeyFifths::ONE_SHARP:
            // SOL
            return std::make_tuple(Step::G, Alter::NATURAL);
        case KeyFifths::TWO_SHARPS:
            // RE
            return std::make_tuple(Step::D, Alter::NATURAL);
        case KeyFifths::THREE_SHARPS:
            // LA
            return std::make_tuple(Step::A, Alter::NATURAL);
        case KeyFifths::FOUR_SHARPS:
            // MI
            return std::make_tuple(Step::E, Alter::NATURAL);
        case KeyFifths::FIVE_SHARPS:
            // SI
            return std::make_tuple(Step::B, Alter::NATURAL);
        case KeyFifths::SIX_SHARPS:
            // FA SHARP
            return std::make_tuple(Step::F, Alter::SHARP);
        case KeyFifths::SEVEN_SHARPS:
            // DO SHARP
            return std::make_tuple(Step::C, Alter::SHARP);
        case KeyFifths::ONE_FLAT:
            // FA
            return std::make_tuple(Step::F, Alter::NATURAL);
        case KeyFifths::TWO_FLATS:
            // SI FLAT
            return std::make_tuple(Step::B, Alter::FLAT);
        case KeyFifths::THREE_FLATS:
            // MI FLAT
            return std::make_tuple(Step::E, Alter::FLAT);
        case KeyFifths::FOUR_FLATS:
            // LA FLAT
            return std::make_tuple(Step::A, Alter::FLAT);
        case KeyFifths::FIVE_FLATS:
            // RE FLAT
            return std::make_tuple(Step::D, Alter::FLAT);
        case KeyFifths::SIX_FLATS:
            // SOL FLAT
            return std::make_tuple(Step::G, Alter::FLAT);
        case KeyFifths::SEVEN_FLATS:
            // DO FLAT
            return std::make_tuple(Step::C, Alter::FLAT);
        default:
            throw std::runtime_error("Input fifth option does not exists");
    }
}


bool isHeightInFifth(int height, KeyFifths fifth) {
    int fifth_no = int(fifth);
    // DO / SI DIESE
    if((height == 0) && (((fifth_no >= -5) && (fifth_no <= 1)) || (fifth_no == 7))) {
        return true;
    // DO DIESE / RE BEMOL
    } else if((height == 1) && ((fifth_no >= 2) || (fifth_no <= -4))) {
        return true;
    // RE
    } else if((height == 2) && ((fifth_no >= -3) && (fifth_no <= 3))) {
        return true;
    // RE DIESE / MI BEMOL
    } else if((height == 3) && ((fifth_no >= 4) || (fifth_no <= -2))) {
        return true;
    // MI / FA BEMOL
    } else if((height == 4) && (((fifth_no >= -1) && (fifth_no <= 5)) || (fifth_no == -7))) {
        return true;
    // FA / MI DIESE
    } else if((height == 5) && (((fifth_no >= -6) && (fifth_no <= 0)) || (fifth_no >= 6))) {
        return true;
    // FA DIESE / SOL BEMOL
    } else if((height == 6) && ((fifth_no >= 1) || (fifth_no <= -5))) {
        return true;
    // SOL
    } else if((height == 7) && ((fifth_no >= -4) && (fifth_no <= 2))) {
        return true;
    // SOL DIESE / LA BEMOL
    } else if((height == 8) && ((fifth_no >= 3) || (fifth_no <= -3))) {
        return true;
    // LA
    } else if((height == 9) && ((fifth_no >= -2) && (fifth_no <= 4))) {
        return true;
    // LA DIESE / SI BEMOL
    } else if((height == 10) && ((fifth_no >= 5) || (fifth_no <= -1))) {
        return true;
    // SI / DO BEMOL
    } else if((height == 11) && (((fifth_no >= 0) && (fifth_no <= 6)) || (fifth_no <= -6))) {
        return true;
    }
    return false;
}

int medianValue(std::vector<int> data) {
    size_t size = data.size();
    if(size == 0) {
        throw std::runtime_error("Cannot get the median value of an empty vector");
    }
    std::sort(data.begin(), data.end());
    if(size % 2) {
        return(data[size / 2]);
    } else {
        return(data[size / 2 - 1] + data[size / 2]) / 2;
    }
}



HeightDetector::HeightDetector() {
    // Constructor
    this->offset = 0.0;
    this->clef = ClefSign::G;
    this->fifth = KeyFifths::ZERO;
    this->mode = KeyMode::MAJOR;
}


HeightDetector::~HeightDetector() {
    // Destructor
}


void HeightDetector::setOffset(int offset) {
    this->offset = offset;
}


void HeightDetector::setFifth(KeyFifths fifth) {
    this->fifth = fifth;
}


void HeightDetector::setClef(ClefSign clef) {
    this->clef = clef;
}


void HeightDetector::setMode(KeyMode mode) {
    this->mode = mode;
}


KeyFifths HeightDetector::getFifth() const {
    return this->fifth;
}


ClefSign HeightDetector::getClef() const {
    return this->clef;
}


KeyMode HeightDetector::getMode() const {
    return this->mode;
}


void HeightDetector::setOptimumOffset(const std::vector<double> & pitchs_st) {
    double precision = 0.005;
    unsigned int nb_offset_tested = floor(1.0 / precision);
    double offset_tested;
    double error_tested;
    double height_simulated;
    double error_min = std::numeric_limits<double>::max();
    double offset_opt = 0;
    for(unsigned int k = 0; k < nb_offset_tested; k++) {
        offset_tested = -0.5 + k * precision;
        error_tested = 0.0;
        // Iterate over all the notes (not rest either simulated notes)
        for(const auto & pitch: pitchs_st) {
            height_simulated = pitch + offset_tested;
            error_tested += std::abs(height_simulated - round(height_simulated));
        }
        if(error_tested < error_min){
            error_min = error_tested;
            offset_opt = offset_tested;
        }
    }
    this->offset = offset_opt;
}


void HeightDetector::findBestFifth(const std::vector<HeightOctave> & notes) {
    std::vector<int> fifths = {0, 1, -1, 2, -2, 3, -3, 4, -4, 5, -5, 6, -6, 7, -7};
    unsigned int nb_sign_temp;
    unsigned int nb_sign_min = std::numeric_limits<unsigned int>::max();
    KeyFifths best_fifth = KeyFifths::ZERO;
    for(auto fifth: fifths) {
        nb_sign_temp = 0;
        for(auto note: notes) {
            if(!isHeightInFifth(note.height, static_cast<KeyFifths>(fifth))) {
                nb_sign_temp += 1;
            }
        }
        if(nb_sign_temp < nb_sign_min) {
            best_fifth = static_cast<KeyFifths>(fifth);
            nb_sign_min = nb_sign_temp;
        }
    }
    this->fifth = best_fifth;
}


int HeightDetector::medianHeight(const std::vector<HeightOctave> & notes) const {
    std::vector<int> list_heights;
    for(const auto & note: notes) {
        list_heights.push_back(note.height + 12 * note.octave);
    }
    int med = medianValue(list_heights);
    return med;
}


void HeightDetector::findBestClef(const std::vector<HeightOctave> & notes) {
    int med_height = this->medianHeight(notes);
    if((double)med_height / 12.0 >= 4) {
        this->clef = ClefSign::G;
    } else {
        this->clef = ClefSign::F;
    }
}

void HeightDetector::findMode(const std::vector<HeightOctave> & notes) {
    // Major scale
    int note_major;
    if((int)this->fifth % 2) {
        note_major = ((int)this->fifth + 6) % 12;
    } else {
        note_major = ((int)this->fifth + 12) % 12;
    }
    int note_minor = (note_major + 9) % 12;
    int leading_tone_note_minor = (note_minor - 1) % 12;
    if(notes[0].height == note_major || notes.back().height == note_major) {
        this->mode = KeyMode::MAJOR;
    } else if(notes[0].height == note_minor || notes[0].height == leading_tone_note_minor || notes.back().height == note_minor){
        this->mode = KeyMode::MINOR;
    }
}


std::vector<HeightOctave> HeightDetector::getHeightAndOctave(const std::vector<double> & pitchs_st) const {
    std::vector<HeightOctave> result;
    for(const auto & pitch: pitchs_st) {
        result.push_back(this->getHeightAndOctave(pitch));
    }
    return result;
}


HeightOctave HeightDetector::getHeightAndOctave(double pitch_st) const{
    HeightOctave result;
    pitch_st += this->offset;
    int octave = (int)floor(pitch_st / 12.0);
    double height = std::fmod(pitch_st, 12.0);
    if(height >= 11.5) {
        octave += 1;
        height = 0;
    } else {
        height = round(height);
    }
    result.height = (int)height;
    result.octave = octave;
    return result;
}


void HeightDetector::fit(const StepResult & step_result) {
    std::vector<double> pitchs_st;
    for(const auto & note: step_result.notes) {
        if(note.is_a_note) {
            pitchs_st.push_back(note.pitch_st);
        }
    }
    this->fit(pitchs_st);
}


void HeightDetector::fit(const std::vector<double> & pitchs_st) {
    this->setOptimumOffset(pitchs_st);
    std::vector<HeightOctave> heights_and_octaves = this->getHeightAndOctave(pitchs_st);
    this->findBestFifth(heights_and_octaves);
    this->findBestClef(heights_and_octaves);
    this->findMode(heights_and_octaves);
}


NoteHeight HeightDetector::transform(double pitch_st) const {
    HeightOctave note = this->getHeightAndOctave(pitch_st);
    return(getStepAlterOctave(note.height, note.octave, this->fifth));
}


std::vector<NoteHeight> HeightDetector::transform(const std::vector<double> & pitchs_st) const {
    std::vector<NoteHeight> result;
    for(const auto & pitch: pitchs_st) {
        result.push_back(this->transform(pitch));
    }
    return result;
}
