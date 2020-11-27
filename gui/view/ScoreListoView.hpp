#ifndef SCORE_LISTO_VIEW
#define SCORE_LISTO_VIEW

#include "PitchDetector.hpp"
#include "McLeodPitchExtractorMethod.hpp"
#include "StepDetector.hpp"
#include "MusicXmlScore.hpp"
#include "RhythmDetector.hpp"
#include "NoteDetector.hpp"
#include "ScoreListoModel.hpp"
#include "HomeWidget.hpp"
#include "PlotWidget.hpp"
#include "AdvancedConverterWidget.hpp"
#include <QWidget>
#include <QMainWindow>
#include <QStackedWidget>


class ScoreListoView: public QMainWindow {
    Q_OBJECT
public:
    ScoreListoView(ScoreListoModel *model, QWidget *parent=nullptr);
    ~ScoreListoView();
private:
    ScoreListoModel *model;
    HomeWidget *home;
    AdvancedConverterWidget *advancedconverter;
    QStackedWidget *stack;
    PlotWidget *pitchplot;
private slots:
    void runFullConversion(const AudioStream & stream);
    void showAdvancedConverter(const AudioStream & stream);
    void showHome();
    void cancelCurrentConversion();
    void downloadPitch();
    void downloadStep();
    void downloadScore();
    void plotPitchResults();
    void plotStepResults();
    void updateProgression(float progression);
    void actionsPitchConversionStarted();
    void actionsStepConversionStarted();
    void actionsNoteConversionStarted();
    void actionsPitchConversionFinished();
    void actionsStepConversionFinished();
    void actionsNoteConversionFinished();
    void actionsConversionFailed();
    void fileOpened();
    void runPitchConversion();
    void runStepConversion();
    void runNoteConversion();
};

#endif /* SCORE_LISTO_VIEW */
