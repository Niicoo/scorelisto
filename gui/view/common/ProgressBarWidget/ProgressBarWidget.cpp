#include "ProgressBarWidget.hpp"
#include <QWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>


ProgressBarWidget::ProgressBarWidget(QWidget *parent/*=nullptr*/): QWidget(parent) {
    /* Widgets */
    this->bar = new QProgressBar();
    this->cancel = new QPushButton("Cancel");
    this->pitchlabel = new QLabel("Pitch conversion");
    this->steplabel = new QLabel("Step conversion");
    this->notelabel = new QLabel("Note conversion");
    this->pitchcursor = new QLabel("");
    this->stepcursor = new QLabel("");
    this->notecursor = new QLabel("");
    this->dl_pitch_button = new QPushButton("Download");
    this->dl_step_button = new QPushButton("Download");
    this->dl_score_button = new QPushButton("Download Score");
    /* Widgets (End) */

    /* Logic */
    this->bar->setRange(0, 100);
    this->cancel->setDisabled(true);
    this->dl_pitch_button->setDisabled(true);
    this->dl_step_button->setDisabled(true);
    this->dl_score_button->setDisabled(true);
    /* Logic (End) */

    /* Architecture */
    QHBoxLayout *pitch_layout = new QHBoxLayout;
    pitch_layout->addWidget(this->pitchcursor);
    pitch_layout->addWidget(this->pitchlabel);
    pitch_layout->addWidget(this->dl_pitch_button);
    pitch_layout->addStretch();
    QHBoxLayout *step_layout = new QHBoxLayout;
    step_layout->addWidget(this->stepcursor);
    step_layout->addWidget(this->steplabel);
    step_layout->addWidget(this->dl_step_button);
    step_layout->addStretch();
    QHBoxLayout *note_layout = new QHBoxLayout;
    note_layout->addWidget(this->notecursor);
    note_layout->addWidget(this->notelabel);
    note_layout->addStretch();
    note_layout->addWidget(this->dl_score_button);
    

    QHBoxLayout *bar_layout = new QHBoxLayout;
    bar_layout->addWidget(this->bar);
    bar_layout->addWidget(this->cancel);
    QVBoxLayout *root_layout = new QVBoxLayout(this);
    root_layout->addStretch();
    root_layout->addLayout(bar_layout);
    root_layout->addLayout(pitch_layout);
    root_layout->addLayout(step_layout);
    root_layout->addLayout(note_layout);
    root_layout->addStretch();
    /* Architecture (End) */

    /* Design */
    this->pitchcursor->setFixedWidth(20);
    this->stepcursor->setFixedWidth(20);
    this->notecursor->setFixedWidth(20);
    this->pitchlabel->setFixedWidth(130);
    this->steplabel->setFixedWidth(130);
    this->notelabel->setFixedWidth(130);
    /* Design (End) */

    /* Connexions */
    connect(this->cancel, SIGNAL (clicked()), this, SIGNAL (cancelClicked()));
    connect(this->dl_pitch_button, SIGNAL (clicked()), this, SIGNAL (downloadPitchClicked()));
    connect(this->dl_step_button, SIGNAL (clicked()), this, SIGNAL (downloadStepClicked()));
    connect(this->dl_score_button, SIGNAL (clicked()), this, SIGNAL (downloadScoreClicked()));
    /* Connexions (End) */
}

ProgressBarWidget::~ProgressBarWidget() {

}

void ProgressBarWidget::clear() {
    this->updateProgression(0.0);
    this->cancel->setDisabled(true);
    this->pitchcursor->setText("");
    this->stepcursor->setText("");
    this->notecursor->setText("");
    this->pitchlabel->setStyleSheet("font-weight: normal; color: black");
    this->steplabel->setStyleSheet("font-weight: normal; color: black");
    this->notelabel->setStyleSheet("font-weight: normal; color: black");
}


void ProgressBarWidget::updateProgression(float progress) {
    this->bar->setValue(static_cast<int>(progress));
}



void ProgressBarWidget::setPitchConversionStarted() {
    this->cancel->setEnabled(true);
    this->dl_pitch_button->setEnabled(false);
    this->dl_step_button->setEnabled(false);
    this->dl_score_button->setEnabled(false);
    this->pitchcursor->setText(">");
    this->stepcursor->setText("");
    this->notecursor->setText("");
}

void ProgressBarWidget::setStepConversionStarted() {
    this->cancel->setEnabled(true);
    this->dl_step_button->setEnabled(false);
    this->dl_score_button->setEnabled(false);
    this->pitchcursor->setText("");
    this->stepcursor->setText(">");
    this->notecursor->setText("");
}

void ProgressBarWidget::setNoteConversionStarted() {
    this->cancel->setEnabled(true);
    this->dl_score_button->setEnabled(false);
    this->pitchcursor->setText("");
    this->stepcursor->setText("");
    this->notecursor->setText(">");
}


void ProgressBarWidget::setPitchConversionFinished() {
    this->cancel->setEnabled(false);
    this->dl_pitch_button->setEnabled(true);
    this->pitchlabel->setStyleSheet("font-weight: bold; color: green");
    this->steplabel->setStyleSheet("font-weight: normal; color: black");
    this->notelabel->setStyleSheet("font-weight: normal; color: black");
}


void ProgressBarWidget::setStepConversionFinished() {
    this->cancel->setEnabled(false);
    this->dl_step_button->setEnabled(true);
    this->pitchlabel->setStyleSheet("font-weight: bold; color: green");
    this->steplabel->setStyleSheet("font-weight: bold; color: green");
    this->notelabel->setStyleSheet("font-weight: normal; color: black");
}


void ProgressBarWidget::setNoteConversionFinished() {
    this->cancel->setEnabled(false);
    this->dl_score_button->setEnabled(true);
    this->pitchlabel->setStyleSheet("font-weight: bold; color: green");
    this->steplabel->setStyleSheet("font-weight: bold; color: green");
    this->notelabel->setStyleSheet("font-weight: bold; color: green");
}


void ProgressBarWidget::setPitchConversionFailed() {
    this->cancel->setEnabled(false);
    this->dl_pitch_button->setEnabled(false);
    this->pitchlabel->setStyleSheet("font-weight: bold; color: red");
}

void ProgressBarWidget::setStepConversionFailed() {
    this->cancel->setEnabled(false);
    this->dl_step_button->setEnabled(false);
    this->steplabel->setStyleSheet("font-weight: bold; color: red");
}

void ProgressBarWidget::setNoteConversionFailed() {
    this->cancel->setEnabled(false);
    this->dl_score_button->setEnabled(false);
    this->notelabel->setStyleSheet("font-weight: bold; color: red");
}
