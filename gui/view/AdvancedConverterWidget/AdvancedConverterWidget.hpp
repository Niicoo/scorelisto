#ifndef ADVANCED_CONVERTER_WIDGET
#define ADVANCED_CONVERTER_WIDGET

// ScoreListo
#include "PitchDetector.hpp"
#include "StepDetector.hpp"
#include "MusicXmlScore.hpp"
#include "ScoreListoModel.hpp"
// Qt
#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QCheckBox>
// Custom widgets
#include "ProgressBarWidget.hpp"
#include "AudioReaderParametersWidget.hpp"
#include "McLeodParametersWidget.hpp"
#include "TimeRangeParametersWidget.hpp"
#include "StepParametersWidget.hpp"
#include "RhythmParametersWidget.hpp"


class AdvancedConverterWidget: public QWidget {
    Q_OBJECT
public:
    AdvancedConverterWidget(QWidget *parent=nullptr);
    ~AdvancedConverterWidget();
    void setStream(const AudioStream & stream);
    ProgressBarWidget *progress_bar;
    AudioReaderParameters getAudioReaderParameters();
    TimeRangeParameters getTimeRangeParameters();
    McLeodParameters getMcLeodParameters();
    StepParameters getStepParameters();
    RhythmParameters getRhythmParameters();
    unsigned int getIndex();
    bool isPlotChecked();
private:
    QPushButton *run_button;
    QPushButton *next_button;
    QPushButton *back_button;
    QCheckBox *plot_check;
    AudioReaderParametersWidget *audio_param;
    McLeodParametersWidget *mcleod_param;
    TimeRangeParametersWidget *range_param;
    StepParametersWidget *step_param;
    RhythmParametersWidget *rhythm_param;
    QStackedWidget *stack;
    AudioStream stream;
    bool model_running;
    Stage model_stage;
    TimeRangeParameters range;
private slots:
    void run();
    void next();
    void back();
    void updateRange(TimeRangeParameters parameters);
    void checkRunButton();
    void checkNextButton();
public slots:
    void clear();
    void setModelRunning(Mode mode);
    void setModelStage(Stage stage);
signals:
    void backHomeClicked();
    void plotResultsChanged(int);
    void pitchConversionClicked();
    void stepConversionClicked();
    void noteConversionClicked();
};

#endif /* ADVANCED_CONVERTER_WIDGET */
