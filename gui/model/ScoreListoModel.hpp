#ifndef SCORE_LISTO_MODEL
#define SCORE_LISTO_MODEL

#include <QtCore>
#include <string>
#include "PitchDetector.hpp"
#include "McLeodPitchExtractorMethod.hpp"
#include "StepDetector.hpp"
#include "MusicXmlScore.hpp"
#include "RhythmDetector.hpp"
#include "NoteDetector.hpp"
#include "AudioReader.hpp"

enum class Mode {
    IDLE = 0,
    RUNNING_FILE = 1,
    RUNNING_PITCH = 2,
    RUNNING_STEP = 3,
    RUNNING_NOTE = 4,
    RUNNING_FULL = 5
};

enum class Stage {
    ZERO = 0,
    FILE = 1,
    PITCH = 2,
    STEP = 3,
    NOTE = 4
};


class ScoreListoModel : public QObject{
    Q_OBJECT
public:
    ScoreListoModel();
    virtual ~ScoreListoModel();

    // Init a new conversion
    void init();
    void init(const PitchResult & result);
    void init(const StepResult & result);
    void reset(Stage stage=Stage::ZERO);

    // Get Results
    std::string getFilepath() const;
    std::vector<AudioStream> getStreams() const;
    PitchResult getPitchResult() const;
    StepResult getStepResult() const;
    MusicXmlScore getMusicXmlScore() const;

    float getProgress() const;
    Mode getMode() const;                       
    Stage getStage() const;
    Stage getMinStage() const;
private:
    // Outputs
    std::string filepath;
    AudioReader *audio_reader;
    PitchResult *output_pitch;
    StepResult *output_step;
    MusicXmlScore *output_score;
    // Futures attributes to get outputs from the threads
    std::future<int> fThread;
    
    Mode mode;
    Stage stage;
    Stage minstage;
    
    std::atomic<float> progress;

    void setMode(Mode mode);
    void setStage(Stage stage);
    void setFilepath(std::string path);
    void setPitchResult(const PitchResult & result);
    void setStepResult(const StepResult & result);
    void setMusicXmlScore(const MusicXmlScore & score);
    void setProgress(float value);
    void setMinStage(Stage stage);
    void runCheck(Stage stage);

    // Methods run in threads
    static int fullConversion(  ScoreListoModel * inst,
                                unsigned int ind_stream,
                                const McLeodParameters & mcleod_parameters,
                                const AudioReaderParameters & reader_parameters,
                                double time_start_s,
                                double time_stop_s,
                                const StepParameters & step_parameters,
                                const RhythmParameters & rhythm_parameters);
    static int fileOpening(ScoreListoModel * inst, std::string path);
    static int pitchConversion( ScoreListoModel * inst,
                                unsigned int ind_stream,
                                const McLeodParameters & mcleod_parameters,
                                const AudioReaderParameters & reader_parameters,
                                double time_start_s,
                                double time_stop_s);
    static int stepConversion(  ScoreListoModel * inst,
                                const StepParameters & step_parameters);
    static int noteConversion(  ScoreListoModel * inst,
                                const RhythmParameters & rhythm_parameters);

    // Check the progression and update result when finish
    static void checkProgression(ScoreListoModel * inst);
public slots:
    // Running conversion stages
    void runFullConversion( unsigned int ind_stream=0,
                            const McLeodParameters & mcleod_parameters=DEFAULT_MC_LEOD_PARAMETERS,
                            const AudioReaderParameters & reader_parameters=DEFAULT_AUDIO_READER_PARAMETERS,
                            double time_start_s=-1.0,
                            double time_stop_s=-1.0,
                            const StepParameters & step_parameters=DEFAULT_STEP_PARAMETERS,
                            const RhythmParameters & rhythm_parameters=DEFAULT_RHYTHM_PARAMETERS);
    void runFileOpening(std::string path);
    void runPitchConversion(unsigned int ind_stream=0,
                            const McLeodParameters & mcleod_parameters=DEFAULT_MC_LEOD_PARAMETERS,
                            const AudioReaderParameters & reader_parameters=DEFAULT_AUDIO_READER_PARAMETERS,
                            double time_start_s=-1.0,
                            double time_stop_s=-1.0);
    void runStepConversion( const StepParameters & parameters=DEFAULT_STEP_PARAMETERS);
    void runNoteConversion( const RhythmParameters & parameters=DEFAULT_RHYTHM_PARAMETERS);
    void cancelConversion();
signals:
    void taskProgressed(float percent);

    void fileOpeningStarted();
    void pitchConversionStarted();
    void stepConversionStarted();
    void noteConversionStarted();

    void fileOpeningFinished();
    void pitchConversionFinished();
    void stepConversionFinished();
    void noteConversionFinished();

    void fileOpeningFailed();
    void pitchConversionFailed();
    void stepConversionFailed();
    void noteConversionFailed();

    void modeChanged(Mode);
    void stageChanged(Stage);
};

#endif /* SCORE_LISTO_MODEL */




