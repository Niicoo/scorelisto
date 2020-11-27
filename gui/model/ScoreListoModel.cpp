#include "ScoreListoModel.hpp"
#include "HistogramStepDetector.hpp"
#include "MusicXmlScore.hpp"
#include "FFmpegAudioReader.hpp"
#include <chrono>
#include <thread>


ScoreListoModel::ScoreListoModel() {
    this->setStage(Stage::ZERO);
    this->setMinStage(Stage::NOTE);
    this->setMode(Mode::IDLE);
    this->audio_reader = nullptr;
    this->output_pitch = nullptr;
    this->output_step = nullptr;
    this->output_score = nullptr;
}


ScoreListoModel::~ScoreListoModel() {
    if(this->audio_reader) {
        delete this->audio_reader;
    }
    if(this->output_pitch) {
        delete this->output_pitch;
    }
    if(this->output_step) {
        delete this->output_step;
    }
    if(this->output_score) {
        delete this->output_score;
    }
}

// Get/Set state
void ScoreListoModel::setMode(Mode mode) {
    this->mode = mode;
    emit this->modeChanged(mode);
}


Mode ScoreListoModel::getMode() const {
    return this->mode;
}


// Get/Set stage
void ScoreListoModel::setStage(Stage stage) {
    this->stage = stage;
    emit this->stageChanged(stage);
}


Stage ScoreListoModel::getStage() const {
    return this->stage;
}


void ScoreListoModel::setMinStage(Stage stage) {
    this->minstage = stage;
}


Stage ScoreListoModel::getMinStage() const {
    return this->minstage;
}


// Set Results Methods
void ScoreListoModel::setFilepath(std::string path) {
    this->filepath = path;
}


void ScoreListoModel::setPitchResult(const PitchResult & result) {
    if(this->output_pitch) {
        delete this->output_pitch;
    }
    this->output_pitch = new PitchResult(result);
}


void ScoreListoModel::setStepResult(const StepResult & result) {
    if(this->output_step) {
        delete this->output_step;
    }
    this->output_step = new StepResult(result);
}

void ScoreListoModel::setMusicXmlScore(const MusicXmlScore & score) {
    if(this->output_score) {
        delete this->output_score;
    }
    this->output_score = new MusicXmlScore(score);
}

// Get Results Methods
std::string ScoreListoModel::getFilepath() const {
    if(this->getStage() < Stage::FILE) {
        throw std::logic_error("File has not been opened yet");  
    }
    return this->filepath;
}


std::vector<AudioStream> ScoreListoModel::getStreams() const {
    if(this->getStage() < Stage::FILE) {
        throw std::logic_error("File has not been opened yet");  
    }
    return this->audio_reader->getStreams();
}


PitchResult ScoreListoModel::getPitchResult() const {
    if(this->getStage() < Stage::PITCH) {
        throw std::logic_error("Pitch conversion has not been running yet");  
    }
    return *this->output_pitch;
}


StepResult ScoreListoModel::getStepResult() const {
    if(this->getStage() < Stage::STEP) {
        throw std::logic_error("Step conversion has not been running yet");  
    }
    return *this->output_step;
}

MusicXmlScore ScoreListoModel::getMusicXmlScore() const {
    if(this->getStage() < Stage::NOTE) {
        throw std::logic_error("Note conversion has not been running yet");
    }
    return *this->output_score;
}

int ScoreListoModel::fileOpening(ScoreListoModel * inst, std::string path) {
    emit inst->fileOpeningStarted();
    emit inst->taskProgressed(0.0);
    try {
        inst->audio_reader = new FFmpegAudioReader(path);
        inst->setFilepath(path);
    } catch (...) {
        emit inst->fileOpeningFailed();
        inst->progress = -1;
        return -1;
    }
    inst->setStage(Stage::FILE);
    emit inst->fileOpeningFinished();
    if(inst->getMode() != Mode::RUNNING_FULL) {
        inst->progress = -1;
    }
    return 0;
}

int ScoreListoModel::pitchConversion(   ScoreListoModel * inst,
                                        unsigned int ind_stream,
                                        const McLeodParameters & mcleod_parameters,
                                        const AudioReaderParameters & reader_parameters,
                                        double time_start_s,
                                        double time_stop_s) {
    emit inst->pitchConversionStarted();
    emit inst->taskProgressed(0.0);
    try {
        // Pitch extractor method
        McLeodPitchExtractorMethod mcleod_method(mcleod_parameters);
        // Pitch detector
        PitchDetector pitch_detector(inst->audio_reader, &mcleod_method);
        PitchResult result = pitch_detector.perform(&inst->progress, reader_parameters, ind_stream, time_start_s, time_stop_s);
        inst->setPitchResult(result);
    } catch (...) {
        if(inst->progress >= 0) {
            emit inst->pitchConversionFailed();
        }
        inst->progress = -1;
        return -1;
    }
    inst->setStage(Stage::PITCH);
    emit inst->pitchConversionFinished();
    if(inst->getMode() != Mode::RUNNING_FULL) {
        inst->progress = -1;
    }
    return 0;
}

int ScoreListoModel::stepConversion(ScoreListoModel * inst, const StepParameters & step_parameters) {
    emit inst->stepConversionStarted();
    emit inst->taskProgressed(0.0);
    try {
        HistogramStepDetector step_detector(*inst->output_pitch);
        StepResult result = step_detector.perform(&inst->progress, step_parameters);
        inst->setStepResult(result);
    } catch (...) {
        if(inst->progress.load() >= 0) {
            emit inst->stepConversionFailed();
        }
        inst->progress = -1;
        return -1;
    }
    inst->setStage(Stage::STEP);
    emit inst->stepConversionFinished();
    if(inst->getMode() != Mode::RUNNING_FULL) {
        inst->progress = -1;
    }
    return 0;
}

int ScoreListoModel::noteConversion(ScoreListoModel * inst, const RhythmParameters & rhythm_parameters) {
    emit inst->noteConversionStarted();
    emit inst->taskProgressed(0.0);
    try {
        NoteDetector note_detector(*inst->output_step);
        MusicXmlScore score = note_detector.perform(&inst->progress, rhythm_parameters);
        inst->setMusicXmlScore(score);
    } catch (...) {
        if(inst->progress.load() >= 0) {
            emit inst->noteConversionFailed();
        }
        inst->progress = -1;
        return -1;
    }
    inst->setStage(Stage::NOTE);
    emit inst->noteConversionFinished();
    inst->progress = -1;
    return 0;
}


int ScoreListoModel::fullConversion(ScoreListoModel * inst,
                                    unsigned int ind_stream,
                                    const McLeodParameters & mcleod_parameters,
                                    const AudioReaderParameters & reader_parameters,
                                    double time_start_s,
                                    double time_stop_s,
                                    const StepParameters & step_parameters,
                                    const RhythmParameters & rhythm_parameters) {
    int ret = inst->pitchConversion(inst, ind_stream, mcleod_parameters, reader_parameters, time_start_s, time_stop_s);
    if(ret == 0) { ret = inst->stepConversion(inst, step_parameters);}
    if(ret == 0) { ret = inst->noteConversion(inst, rhythm_parameters);}
}


void ScoreListoModel::checkProgression(ScoreListoModel * inst) {
    bool control = true;
    float progression;
    while(control) {
        progression = inst->progress.load();
        if(progression < 0) {
            control = false;
            int ret_thread = inst->fThread.get();
            if(ret_thread < 0) {
                emit inst->taskProgressed(0.0);
            } else {
                emit inst->taskProgressed(100.0);
            }
        } else {
            emit inst->taskProgressed(progression);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    inst->setMode(Mode::IDLE);
}

// Init a new conversion
void ScoreListoModel::init() {
    this->reset(Stage::ZERO);
}

void ScoreListoModel::init(const PitchResult & result) {
    this->reset(Stage::ZERO);
    this->setStage(Stage::PITCH);
    this->setMinStage(Stage::PITCH);
    this->setPitchResult(result);
}

void ScoreListoModel::init(const StepResult & result) {
    this->reset(Stage::ZERO);
    this->setStage(Stage::STEP);
    this->setMinStage(Stage::STEP);
    this->setStepResult(result);
}

void ScoreListoModel::reset(Stage stage/*=Stage::ZERO*/) {
    if(this->getMode() != Mode::IDLE) {
        throw std::logic_error("The model is already running a task");
    }
    if((this->getMinStage() >= stage) && (stage != Stage::ZERO)) {
        throw std::logic_error("Stage inferior to the minimum stage data of the model"); 
    }
    this->setStage(stage);
    if((stage < Stage::NOTE) && this->output_score) {
        delete this->output_score;
        this->output_score = nullptr;
    }
    if((stage < Stage::STEP) && this->output_step) {
        delete this->output_step;
        this->output_step = nullptr;
    }
    if((stage < Stage::PITCH) && this->output_pitch) {
        delete this->output_pitch;
        this->output_pitch = nullptr;
    }
    if((stage < Stage::FILE) && this->audio_reader) {
        delete this->audio_reader;
        this->audio_reader = nullptr;
        this->setFilepath("");
    }
}

// Running conversion stages
void ScoreListoModel::runFileOpening(std::string path) {
    this->reset();
    this->setMinStage(Stage::ZERO);
    this->runCheck(Stage::FILE);
    this->setMode(Mode::RUNNING_FILE);
    std::thread(&ScoreListoModel::checkProgression, this).detach();
    this->fThread = std::async(std::launch::async, this->fileOpening, this, path);
}


void ScoreListoModel::runFullConversion(unsigned int ind_stream/*=0*/,
                                        const McLeodParameters & mcleod_parameters/*=DEFAULT_MC_LEOD_PARAMETERS*/,
                                        const AudioReaderParameters & reader_parameters/*=DEFAULT_AUDIO_READER_PARAMETERS*/,
                                        double time_start_s/*=-1.0*/,
                                        double time_stop_s/*=-1.0*/,
                                        const StepParameters & step_parameters/*=DEFAULT_STEP_PARAMETERS*/,
                                        const RhythmParameters & rhythm_parameters/*=DEFAULT_RHYTHM_PARAMETERS*/) {
    this->reset(Stage::FILE);
    this->setMinStage(Stage::ZERO);
    this->runCheck(Stage::FILE);
    this->setMode(Mode::RUNNING_FULL);
    std::thread(&ScoreListoModel::checkProgression, this).detach();
    this->fThread = std::async( std::launch::async,
                                this->fullConversion,
                                this,
                                ind_stream,
                                mcleod_parameters,
                                reader_parameters,
                                time_start_s,
                                time_stop_s,
                                step_parameters,
                                rhythm_parameters);
}


void ScoreListoModel::runPitchConversion(   unsigned int ind_stream/*=0*/,
                                            const McLeodParameters & mcleod_parameters/*=DEFAULT_MC_LEOD_PARAMETERS*/,
                                            const AudioReaderParameters & reader_parameters/*=DEFAULT_AUDIO_READER_PARAMETERS*/,
                                            double time_start_s/*=-1.0*/,
                                            double time_stop_s/*=-1.0*/) {
    this->runCheck(Stage::PITCH);
    this->setMode(Mode::RUNNING_PITCH);
    std::thread(&ScoreListoModel::checkProgression, this).detach();
    this->fThread = std::async( std::launch::async,
                                this->pitchConversion,
                                this,
                                ind_stream,
                                mcleod_parameters,
                                reader_parameters,
                                time_start_s,
                                time_stop_s);
}


void ScoreListoModel::runStepConversion(const StepParameters & parameters/*=DEFAULT_STEP_PARAMETERS*/) {
    this->runCheck(Stage::STEP);
    this->setMode(Mode::RUNNING_STEP);
    std::thread(&ScoreListoModel::checkProgression, this).detach();
    this->fThread = std::async(std::launch::async, this->stepConversion, this, parameters);
}


void ScoreListoModel::runNoteConversion(const RhythmParameters & parameters/*=DEFAULT_RHYTHM_PARAMETERS*/) {
    this->runCheck(Stage::NOTE);
    this->setMode(Mode::RUNNING_NOTE);
    std::thread(&ScoreListoModel::checkProgression, this).detach();
    this->fThread = std::async(std::launch::async, this->noteConversion, this, parameters);
}


void ScoreListoModel::cancelConversion() {
    this->progress = -1;
}


void ScoreListoModel::runCheck(Stage stage) {
    if(stage == Stage::ZERO) {
        throw std::logic_error("invalid input argument `stage`"); 
    }
    if(this->getMinStage() >= stage) {
        throw std::logic_error("The model does not contain the data to run this task"); 
    }
    if(this->getMode() != Mode::IDLE) {
        throw std::logic_error("The model is already running a task");
    }
    if((int)this->getStage() >= (int)stage) {
        this->reset(stage);
    } else if((int)this->getStage() < ((int)stage - 1)) {
        throw std::logic_error("You must first run the previous stage conversion"); 
    }
    this->progress = 0.0;
}
