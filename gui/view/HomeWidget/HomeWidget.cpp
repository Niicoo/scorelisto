#include "ScoreListoModel.hpp"
#include "HomeWidget.hpp"
#include "RecorderWidget.hpp"
#include "FileSelectorWidget.hpp"
#include "ProgressBarWidget.hpp"
#include "StreamSelectorWidget.hpp"
#include "AudioPlayerWidget.hpp"
#include "ScoreListoModel.hpp"
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QMessageBox>


HomeWidget::HomeWidget(QWidget *parent/*=nullptr*/): QWidget(parent) {
    /* Widgets */
    this->recorder = new RecorderWidget(this);
    this->fileselector = new FileSelectorWidget(this);
    this->streamselector = new StreamSelectorWidget(this);
    this->player = new AudioPlayerWidget(this);
    this->convert_button = new QPushButton("Convert", this);
    this->advanced_opt = new QCheckBox("Advanced conversion", this);
    this->progress_bar = new ProgressBarWidget(this);
    /* Widgets (End) */

    /* Logic */
    this->convert_button->setEnabled(false);
    this->advanced_opt->setChecked(false);
    /* Logic (End) */

    /* Design */
    this->convert_button->setFixedSize(150, 40);
    /* Design (End) */

    /* Architecture */
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addWidget(this->fileselector, 0, Qt::AlignCenter);
    main_layout->addWidget(this->recorder);
    main_layout->addWidget(this->streamselector, 0, Qt::AlignCenter);
    main_layout->addStretch();
    main_layout->addWidget(this->convert_button, 0, Qt::AlignCenter);
    main_layout->addWidget(this->advanced_opt, 0, Qt::AlignCenter);
    main_layout->addWidget(this->progress_bar);
    main_layout->addStretch();
    /* Architecture (End) */

    /* Connexions */
    connect(this->convert_button, SIGNAL (clicked()),this, SLOT (emitConversion()));
    connect(this->fileselector, SIGNAL (fileSelected(QString)), this, SLOT (fileOpened(QString)));
    connect(this->streamselector, SIGNAL (indexUpdated(int)), this, SLOT (updatePlayer()));
    /* Connexions (End) */
}


HomeWidget::~HomeWidget() {

}


void HomeWidget::clear() {
    this->recorder->clear();
    this->fileselector->clear();
    this->streamselector->clear();
    this->player->clear();
    this->convert_button->setDisabled(true);
}


void HomeWidget::updatePlayer() {

}


void HomeWidget::emitConversion() {
    AudioStream stream = this->streamselector->getCurrentStream();
    this->player->stop();
    if(this->advanced_opt->isChecked()) {
        emit this->advancedConversionClicked(stream);
    } else {
        emit this->directConversionClicked(stream);
    }
}


void HomeWidget::fileOpened(QString filename) {
    this->streamselector->clear();
    this->convert_button->setDisabled(true);
    this->fileselector->setDisabled(true);
    emit this->runFileOpening(filename.toStdString());
}


void HomeWidget::fileLoaded(std::vector<AudioStream> streams) {
    this->streamselector->updateStreams(streams);
    this->fileselector->setEnabled(true);
    this->convert_button->setEnabled(true);
}

void HomeWidget::fileFailed() {
    this->fileselector->setEnabled(true);
    this->convert_button->setEnabled(false);
}


void HomeWidget::enableConvertButton() {
    this->convert_button->setEnabled(true);
}


void HomeWidget::disableConvertButton() {
    this->convert_button->setDisabled(true);
}


