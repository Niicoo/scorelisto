#include "ScoreListoView.hpp"
#include "AdvancedConverterWidget.hpp"
#include "HomeWidget.hpp"
#include "PitchDetector.hpp"
#include "StepDetector.hpp"
#include "RhythmDetector.hpp"
#include "AudioReader.hpp"
#include <fstream>

// Qt Libraries
#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QtSvg>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>



ScoreListoView::ScoreListoView(ScoreListoModel *model, QWidget *parent/*=nullptr*/): QMainWindow(parent) {
    this->model = model;
    this->setWindowTitle("ScoreListo");
    QString favicon_path = QCoreApplication::applicationDirPath() + "/resources/icons/favicon.svg";
    this->setWindowIcon(QIcon(favicon_path));

    /* Widgets */
    QString appname_path = QCoreApplication::applicationDirPath() + "/resources/icons/appname.svg";
    QSvgWidget *appname = new QSvgWidget(appname_path);
    this->advancedconverter = new AdvancedConverterWidget(this);
    this->home = new HomeWidget(this);
    this->stack = new QStackedWidget(this);
    this->stack->addWidget(this->home);
    this->stack->addWidget(this->advancedconverter);
    this->pitchplot = nullptr;
    /* Widgets (End) */

    /* Design */
    appname->setFixedSize(300, 50);
    /* Design (End) */

    /* Architecture (Begin) */
    QHBoxLayout *applayout = new QHBoxLayout;
    applayout->addStretch();
    applayout->addWidget(appname);
    applayout->addStretch();
    QVBoxLayout *root_layout = new QVBoxLayout;
    root_layout->setAlignment(Qt::AlignHCenter);
    root_layout->addLayout(applayout);
    root_layout->addWidget(this->stack);
    QWidget *tmp = new QWidget(this);
    tmp->setLayout(root_layout);
    this->setCentralWidget(tmp);
    /* Architecture (End) */

    /* Connexion */
    connect(this->advancedconverter, SIGNAL (plotResultsChanged(int)), this, SLOT (plotPitchResults()));
    connect(this->advancedconverter, SIGNAL (plotResultsChanged(int)), this, SLOT (plotStepResults()));
    connect(this->advancedconverter, SIGNAL (backHomeClicked()), this, SLOT (cancelCurrentConversion()));
    connect(this->advancedconverter, SIGNAL (backHomeClicked()), this->home, SLOT (clear()));
    connect(this->advancedconverter, SIGNAL (backHomeClicked()), this, SLOT (showHome()));

    // Progress bars
    connect(this->advancedconverter->progress_bar, SIGNAL (cancelClicked()), this, SLOT (cancelCurrentConversion()));
    connect(this->advancedconverter->progress_bar, SIGNAL (downloadPitchClicked()), this, SLOT (downloadPitch()));
    connect(this->advancedconverter->progress_bar, SIGNAL (downloadStepClicked()), this, SLOT (downloadStep()));
    connect(this->advancedconverter->progress_bar, SIGNAL (downloadScoreClicked()), this, SLOT (downloadScore()));
    connect(this->home->progress_bar, SIGNAL (cancelClicked()), this, SLOT (cancelCurrentConversion()));
    connect(this->home->progress_bar, SIGNAL (downloadPitchClicked()), this, SLOT (downloadPitch()));
    connect(this->home->progress_bar, SIGNAL (downloadStepClicked()), this, SLOT (downloadStep()));
    connect(this->home->progress_bar, SIGNAL (downloadScoreClicked()), this, SLOT (downloadScore()));

    connect(this->home, SIGNAL (runFileOpening(std::string)), this->model, SLOT (runFileOpening(std::string)));
    connect(this->home, SIGNAL (directConversionClicked(AudioStream)), this, SLOT (runFullConversion(AudioStream)));
    connect(this->home, SIGNAL (advancedConversionClicked(AudioStream)), this, SLOT (showAdvancedConverter(AudioStream)));

    connect(this->advancedconverter, SIGNAL (pitchConversionClicked()), this, SLOT (runPitchConversion()));
    connect(this->advancedconverter, SIGNAL (stepConversionClicked()), this, SLOT (runStepConversion()));
    connect(this->advancedconverter, SIGNAL (noteConversionClicked()), this, SLOT (runNoteConversion()));

    qRegisterMetaType<Mode>("Mode");
    qRegisterMetaType<Stage>("Stage");
    connect(this->model, SIGNAL (modeChanged(Mode)), this->advancedconverter, SLOT (setModelRunning(Mode)));
    connect(this->model, SIGNAL (stageChanged(Stage)), this->advancedconverter, SLOT (setModelStage(Stage)));

    connect(this->model, SIGNAL (taskProgressed(float)), this, SLOT (updateProgression(float)));

    connect(this->model, SIGNAL (pitchConversionStarted()), this, SLOT (actionsPitchConversionStarted()));
    connect(this->model, SIGNAL (stepConversionStarted()), this, SLOT (actionsStepConversionStarted()));
    connect(this->model, SIGNAL (noteConversionStarted()), this, SLOT (actionsNoteConversionStarted()));

    connect(this->model, SIGNAL (pitchConversionFinished()), this, SLOT (actionsPitchConversionFinished()));
    connect(this->model, SIGNAL (stepConversionFinished()), this, SLOT (actionsStepConversionFinished()));
    connect(this->model, SIGNAL (noteConversionFinished()), this, SLOT (actionsNoteConversionFinished()));

    connect(this->model, SIGNAL (pitchConversionFailed()), this, SLOT (actionsConversionFailed()));
    connect(this->model, SIGNAL (stepConversionFailed()), this, SLOT (actionsConversionFailed()));
    connect(this->model, SIGNAL (noteConversionFailed()), this, SLOT (actionsConversionFailed()));

    connect(this->model, SIGNAL (fileOpeningFailed()), this->home, SLOT (fileFailed()));
    connect(this->model, SIGNAL (fileOpeningFinished()), this, SLOT (fileOpened()));
    /* Connexion (End) */
}


ScoreListoView::~ScoreListoView() {

}


void ScoreListoView::showHome() {
    this->stack->setCurrentIndex(0);
}


void ScoreListoView::runFullConversion(const AudioStream & stream) {
    this->model->runFullConversion(stream.ind_stream);
}


void ScoreListoView::showAdvancedConverter(const AudioStream & stream) {
    this->advancedconverter->setStream(stream);
    this->stack->setCurrentIndex(1);
}


void ScoreListoView::cancelCurrentConversion() {
    if(this->model->getMode() == Mode::IDLE) {
        return;
    }
    this->model->cancelConversion();
    QMessageBox messageBox(QMessageBox::Information, "Wait", "Please wait while the conversion is stopped");
    messageBox.setModal(true);
    messageBox.setFixedSize(500, 200);
    messageBox.open();
    bool running = true;
    while(running) {
        running = (this->model->getMode() != Mode::IDLE);
    }
    messageBox.close();
}


void ScoreListoView::updateProgression(float progression) {
    switch(this->stack->currentIndex()) {
        case 0: this->home->progress_bar->updateProgression(progression); break;
        case 1: this->advancedconverter->progress_bar->updateProgression(progression); break;
        default: throw std::logic_error("Invalid stack index while receiving progress update");
    }
}

void ScoreListoView::actionsPitchConversionStarted() {
    switch(this->stack->currentIndex()) {
        case 0: this->home->progress_bar->setPitchConversionStarted(); break;
        case 1: this->advancedconverter->progress_bar->setPitchConversionStarted(); break;
        default: throw std::logic_error("Invalid stack index when pitch conversion started");
    }
}


void ScoreListoView::actionsStepConversionStarted() {
    switch(this->stack->currentIndex()) {
        case 0: this->home->progress_bar->setStepConversionStarted(); break;
        case 1: this->advancedconverter->progress_bar->setStepConversionStarted(); break;
        default: throw std::logic_error("Invalid stack index when step conversion started");
    }
}


void ScoreListoView::actionsNoteConversionStarted() {
    switch(this->stack->currentIndex()) {
        case 0: this->home->progress_bar->setNoteConversionStarted(); break;
        case 1: this->advancedconverter->progress_bar->setNoteConversionStarted(); break;
        default: throw std::logic_error("Invalid stack index when rhythm conversion started");
    }
}


void ScoreListoView::actionsPitchConversionFinished() {
    switch(this->stack->currentIndex()) {
        case 0:
            this->home->progress_bar->setPitchConversionFinished();
            break;
        case 1:
            this->advancedconverter->progress_bar->setPitchConversionFinished();
            this->plotPitchResults();
            break;
        default: throw std::logic_error("Invalid stack index when pitch conversion finished");
    }
}


void ScoreListoView::actionsStepConversionFinished() {
    switch(this->stack->currentIndex()) {
        case 0:
            this->home->progress_bar->setStepConversionFinished();
            break;
        case 1:
            this->advancedconverter->progress_bar->setStepConversionFinished();
            this->plotStepResults();
            break;
        default: throw std::logic_error("Invalid stack index when step conversion finished");
    }
}


void ScoreListoView::actionsNoteConversionFinished() {
    switch(this->stack->currentIndex()) {
        case 0:
            this->home->progress_bar->setNoteConversionFinished();
            break;
        case 1:
            this->advancedconverter->progress_bar->setNoteConversionFinished();
            break;
        default: throw std::logic_error("Invalid stack index when note conversion finished");
    }
}


void ScoreListoView::actionsConversionFailed() {
    QMessageBox msg;
    msg.setFixedSize(500, 200);
    msg.critical(this, "Error", "An error occured during the conversion.");
}


void ScoreListoView::fileOpened() {
    if(this->stack->currentIndex() != 0) {
        throw std::logic_error("Invalid stack index during file opening");
    }
    this->home->fileLoaded(this->model->getStreams());
}


void ScoreListoView::runPitchConversion() {
    unsigned int index = this->advancedconverter->getIndex();
    AudioReaderParameters audioparam = this->advancedconverter->getAudioReaderParameters();
    McLeodParameters mcleodparam = this->advancedconverter->getMcLeodParameters();
    TimeRangeParameters rangeparam = this->advancedconverter->getTimeRangeParameters();
    this->model->runPitchConversion(index,
                                    mcleodparam,
                                    audioparam,
                                    rangeparam.min_s,
                                    rangeparam.max_s);
}


void ScoreListoView::runStepConversion() {
    StepParameters parameters = this->advancedconverter->getStepParameters();
    this->model->runStepConversion(parameters);
}


void ScoreListoView::runNoteConversion() {
    RhythmParameters parameters = this->advancedconverter->getRhythmParameters();
    this->model->runNoteConversion(parameters);
}


void ScoreListoView::plotPitchResults() {
    if(!this->advancedconverter->isPlotChecked()) {
        if(this->pitchplot) {
            this->pitchplot->clear();
            delete this->pitchplot;
            this->pitchplot = nullptr;
        }
        return;
    }
    if(!this->pitchplot) {
        this->pitchplot = new PlotWidget;
    }
    this->pitchplot->show();
    if(this->model->getStage() >= Stage::PITCH) {
        this->pitchplot->updatePitchResult(this->model->getPitchResult());
    }
}


void ScoreListoView::plotStepResults() {
    if(!this->advancedconverter->isPlotChecked()) {
        if(this->pitchplot) {
            delete this->pitchplot;
            this->pitchplot = nullptr;
        }
        return;
    }
    if(!this->pitchplot) {
        throw std::logic_error("Trying to plot step results on an empty widget");
    }
    this->pitchplot->show();
    if(!this->pitchplot->isPitchResultsPlotted()) {
        throw std::logic_error("Trying to plot step results whereas pitch has not been plotted yet");
    }
    if(this->model->getStage() >= Stage::STEP) {
        this->pitchplot->updateStepResult(this->model->getStepResult());
    }
}



void ScoreListoView::downloadPitch() {

}


void ScoreListoView::downloadStep() {

}


void ScoreListoView::downloadScore() {
    if(this->model->getStage() != Stage::NOTE ) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","The score is not ready to be downloaded");
        messageBox.setFixedSize(500, 200);
        messageBox.exec();
        return;
    }
    QFileDialog *SaveDialog = new QFileDialog();
    SaveDialog->setNameFilter("XML files (*.xml)");
    SaveDialog->setDefaultSuffix("xml");
    SaveDialog->setAcceptMode(QFileDialog::AcceptSave);
    if(SaveDialog->exec()) {
        QString FilePath = SaveDialog->selectedFiles()[0];
        MusicXmlScore score = this->model->getMusicXmlScore();
        std::ofstream outfile;
        outfile.open(FilePath.toStdString());
        if(outfile.fail()) {
            QMessageBox messageBox;
            messageBox.critical(0,"Error","An error occured to write the score to the selected path, please select a new file path");
            messageBox.setFixedSize(500, 200);
            messageBox.exec();
        }
        outfile << score.toString();
        outfile.close();
    }
}

