#include "AdvancedConverterWidget.hpp"
// ScoreListo
#include "PitchDetector.hpp"
#include "StepDetector.hpp"
#include "MusicXmlScore.hpp"
#include "ScoreListoModel.hpp"
// Qt
#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
// Custom widgets
#include "ProgressBarWidget.hpp"
#include "AudioReaderParametersWidget.hpp"
#include "McLeodParametersWidget.hpp"
#include "TimeRangeParametersWidget.hpp"
#include "StepParametersWidget.hpp"
#include "RhythmParametersWidget.hpp"


AdvancedConverterWidget::AdvancedConverterWidget(QWidget *parent): QWidget(parent) {
    this->range.max_s = -1.0;
    this->range.min_s = -1.0;
    /* Widgets */
    this->run_button = new QPushButton("Run", this);
    this->next_button = new QPushButton("Next", this);
    this->back_button = new QPushButton("Back", this);
    this->plot_check = new QCheckBox("Plot results", this);
    this->progress_bar = new ProgressBarWidget(this);
    this->audio_param = new AudioReaderParametersWidget(this);
    this->mcleod_param = new McLeodParametersWidget(this);
    this->range_param = new TimeRangeParametersWidget(this);
    this->step_param = new StepParametersWidget(this);
    this->rhythm_param = new RhythmParametersWidget(this);
    this->stack = new QStackedWidget(this);
    /* Widgets (End) */

    /* Architecture */
    this->run_button->setFixedSize(120, 50);
    QVBoxLayout *pitch_param_layout = new QVBoxLayout;
    pitch_param_layout->addStretch();
    pitch_param_layout->addWidget(this->audio_param);
    pitch_param_layout->addWidget(this->range_param);
    pitch_param_layout->addWidget(this->mcleod_param);
    pitch_param_layout->addStretch();
    QWidget *pitchparam_widget = new QWidget(this);
    pitchparam_widget->setLayout(pitch_param_layout);
    QVBoxLayout *step_param_layout = new QVBoxLayout;
    step_param_layout->addStretch();
    step_param_layout->addWidget(this->step_param);
    step_param_layout->addStretch();
    QWidget *stepparam_widget = new QWidget(this);
    stepparam_widget->setLayout(step_param_layout);
    this->stack->addWidget(pitchparam_widget);
    this->stack->addWidget(stepparam_widget);
    this->stack->addWidget(this->rhythm_param);    
    QHBoxLayout *back_next_layout = new QHBoxLayout;
    back_next_layout->addWidget(this->back_button);
    back_next_layout->addStretch();
    back_next_layout->addWidget(this->next_button);
    QVBoxLayout *run_layout = new QVBoxLayout;
    run_layout->addStretch();
    run_layout->addWidget(this->run_button, 0, Qt::AlignCenter);
    run_layout->addWidget(this->plot_check, 0, Qt::AlignCenter);
    run_layout->addStretch();
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addWidget(this->stack);
    main_layout->addLayout(run_layout);
    main_layout->addWidget(progress_bar);
    main_layout->addLayout(back_next_layout);
    /* Architecture (End) */

    /* Design */
    /* Design (End) */

    /* Logic */
    this->plot_check->setChecked(true);
    this->model_running = false;
    this->audio_param->setDefaultParameters();
    this->mcleod_param->setDefaultParameters();
    this->range_param->setDefaultParameters();
    this->step_param->setDefaultParameters();
    this->rhythm_param->setDefaultParameters();
    /* Logic (End) */

    /* Connexions */
    connect(this, SIGNAL (backHomeClicked()),this, SLOT (clear()));
    connect(this->plot_check, SIGNAL (stateChanged(int)),this, SIGNAL (plotResultsChanged(int)));
    connect(this->back_button, SIGNAL (clicked()),this, SLOT (back()));
    connect(this->run_button, SIGNAL (clicked()),this, SLOT (run()));
    connect(this->next_button, SIGNAL (clicked()),this, SLOT (next()));
    connect(this->audio_param, SIGNAL (parametersReady()), this, SLOT (checkRunButton()));
    connect(this->mcleod_param, SIGNAL (parametersReady()), this, SLOT (checkRunButton()));
    connect(this->range_param, SIGNAL (parametersReady()), this, SLOT (checkRunButton()));
    connect(this->step_param, SIGNAL (parametersReady()), this, SLOT (checkRunButton()));
    connect(this->rhythm_param, SIGNAL (parametersReady()), this, SLOT (checkRunButton()));
    connect(this->audio_param, SIGNAL (parametersNotReady()), this, SLOT (checkRunButton()));
    connect(this->mcleod_param, SIGNAL (parametersNotReady()), this, SLOT (checkRunButton()));
    connect(this->range_param, SIGNAL (parametersNotReady()), this, SLOT (checkRunButton()));
    connect(this->step_param, SIGNAL (parametersNotReady()), this, SLOT (checkRunButton()));
    connect(this->rhythm_param, SIGNAL (parametersNotReady()), this, SLOT (checkRunButton()));
    connect(this->stack, SIGNAL (currentChanged(int)), this, SLOT (checkRunButton()));
    connect(this->stack, SIGNAL (currentChanged(int)), this, SLOT (checkNextButton()));
    /* Connexions (End) */
}


AdvancedConverterWidget::~AdvancedConverterWidget() {

}


bool AdvancedConverterWidget::isPlotChecked() {
    return this->plot_check->isChecked();
}


void AdvancedConverterWidget::setModelRunning(Mode mode) {
    this->model_running = (mode != Mode::IDLE);
    this->checkRunButton();
    this->checkNextButton();
}


void AdvancedConverterWidget::setModelStage(Stage stage) {
    this->model_stage = stage;
    this->checkNextButton();
}


void AdvancedConverterWidget::clear() {
    this->progress_bar->clear();
    this->audio_param->setDefaultParameters();
    this->mcleod_param->setDefaultParameters();
    this->range_param->setDefaultParameters();
    this->step_param->setDefaultParameters();
    this->rhythm_param->setDefaultParameters();
}


void AdvancedConverterWidget::updateRange(TimeRangeParameters parameters) {
    this->range = parameters;
}


void AdvancedConverterWidget::setStream(const AudioStream & stream) {
    this->stream = stream;
    this->range_param->setValidatorRange(0, stream.duration_s);
}


AudioReaderParameters AdvancedConverterWidget::getAudioReaderParameters() {
    return this->audio_param->getParameters();
}

TimeRangeParameters AdvancedConverterWidget::getTimeRangeParameters() {
    return this->range_param->getParameters();
}

McLeodParameters AdvancedConverterWidget::getMcLeodParameters() {
    return this->mcleod_param->getParameters();
}

StepParameters AdvancedConverterWidget::getStepParameters() {
    return this->step_param->getParameters();
}

RhythmParameters AdvancedConverterWidget::getRhythmParameters() {
    return this->rhythm_param->getParameters();
}

unsigned int AdvancedConverterWidget::getIndex() {
    return this->stream.ind_stream;
}

void AdvancedConverterWidget::run() {
    switch(this->stack->currentIndex()) {
        case 0: emit this->pitchConversionClicked(); break;
        case 1: emit this->stepConversionClicked(); break;
        case 2: emit this->noteConversionClicked(); break;
        default: throw std::logic_error("Invalid stack index");
    }
}


void AdvancedConverterWidget::next() {
    switch(this->stack->currentIndex()) {
        case 0: this->stack->setCurrentIndex(1); break;
        case 1: this->stack->setCurrentIndex(2); break;
        case 2: emit this->backHomeClicked(); break;
        default: throw std::logic_error("Invalid stack index");
    }
}


void AdvancedConverterWidget::back() {
    switch(this->stack->currentIndex()) {
        case 0: {
            QMessageBox messageBox;
            messageBox.setFixedSize(500, 200);
            auto reply = messageBox.question(this, "Cancel Confirmation", "Are you sure you want to go back home ? All unsaved results will be lost");
            if(reply == QMessageBox::Yes) {
                emit this->backHomeClicked();
                this->clear();
            }
            break;
        }
        case 1: this->stack->setCurrentIndex(0); break;
        case 2: this->stack->setCurrentIndex(1); break;
        default: throw std::logic_error("Invalid stack index");
    }
}


void AdvancedConverterWidget::checkRunButton() {
    if(this->model_running) {
        this->run_button->setDisabled(true);
        return;
    }
    switch(this->stack->currentIndex()) {
        case 0:
            this->run_button->setEnabled(
                this->audio_param->isReady() &&
                this->mcleod_param->isReady() &&
                this->range_param->isReady());
            break;
        case 1:
            this->run_button->setEnabled(this->step_param->isReady());
            break;
        case 2:
            this->run_button->setEnabled(this->rhythm_param->isReady());
            break;
        default: throw std::logic_error("Invalid stack index");
    }
}


void AdvancedConverterWidget::checkNextButton() {
    switch(this->stack->currentIndex()) {
        case 0:
            this->next_button->setVisible(true);
            this->next_button->setEnabled(
                !this->model_running &&
                (this->model_stage >= Stage::PITCH));
            break;
        case 1:
            this->next_button->setVisible(true);
            this->next_button->setEnabled(
                !this->model_running &&
                (this->model_stage >= Stage::STEP));
            break;
        case 2:
            this->next_button->setVisible(false);
            break;
        default: throw std::logic_error("Invalid stack index");
    }
}
