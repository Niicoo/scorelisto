#include "AudioReaderParametersWidget.hpp"
#include "AudioReader.hpp"

#include <QWidget>
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QGroupBox>
#include <QString>

#include <QHBoxLayout>
#include <QVBoxLayout>

AudioReaderParametersWidget::AudioReaderParametersWidget(QWidget *parent/*=nullptr*/): QGroupBox("Main Parameters", parent) {
    /* Widgets */
    this->resampled = new QCheckBox("Resample:", this);
    this->windows_size = new QLineEdit;
    this->period = new QLineEdit;
    this->resample_rate = new QLineEdit;
    QLabel *windows_size_unit = new QLabel("[ms]");
    QLabel *period_unit = new QLabel("[ms]");
    QLabel *resample_rate_unit = new QLabel("[Hz]");

    /* Widgets (End) */

    /* Architecture */
    this->windows_size->setFixedWidth(80);
    this->period->setFixedWidth(80);
    this->resample_rate->setFixedWidth(80);
    windows_size_unit->setFixedWidth(35);
    period_unit->setFixedWidth(35);
    resample_rate_unit->setFixedWidth(35);
    QHBoxLayout *ws_layout = new QHBoxLayout;
    ws_layout->addWidget(new QLabel("Windows size:"));
    ws_layout->addStretch();
    ws_layout->addWidget(this->windows_size);
    ws_layout->addWidget(windows_size_unit);
    QHBoxLayout *period_layout = new QHBoxLayout;
    period_layout->addWidget(new QLabel("Period:"));
    period_layout->addStretch();
    period_layout->addWidget(this->period);
    period_layout->addWidget(period_unit);
    QHBoxLayout *rate_layout = new QHBoxLayout;
    rate_layout->addWidget(this->resampled);
    rate_layout->addStretch();
    rate_layout->addWidget(this->resample_rate);
    rate_layout->addWidget(resample_rate_unit);
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addLayout(ws_layout);
    main_layout->addLayout(period_layout);
    main_layout->addLayout(rate_layout);
    /* Architecture (End) */

    /* Logic */
    this->ready = false;
    this->resampled->setChecked(false);
    this->resample_rate->setDisabled(true);
    this->windows_size->setValidator(new QDoubleValidator(0.0, 10000.0, 2));        // Milliseconds
    this->period->setValidator(new QDoubleValidator(0.0, 1000.0, 4));               // Milliseconds
    this->resample_rate->setValidator(new QIntValidator(1, 48000));                 // Hertz
    /* Logic (End) */

    /* Connexions */
    connect(this->resampled, SIGNAL (stateChanged(int)), this, SLOT (enableResampling(int)));
    connect(this->resampled, SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->windows_size, SIGNAL (textEdited(QString)), this, SLOT (checkParameters()));
    connect(this->period, SIGNAL (textEdited(QString)), this, SLOT (checkParameters()));
    connect(this->resample_rate, SIGNAL (textEdited(QString)), this, SLOT (checkParameters()));
    /* Connexions (End) */
}

AudioReaderParametersWidget::~AudioReaderParametersWidget() {

}


void AudioReaderParametersWidget::setDefaultParameters() {
    this->setParameters(DEFAULT_AUDIO_READER_PARAMETERS);
}


bool AudioReaderParametersWidget::isReady() {
    return this->ready;
}


void AudioReaderParametersWidget::setParameters(const AudioReaderParameters & parameters) {
    QLocale *loc = new QLocale();
    this->windows_size->setText(loc->toString(parameters.windowstimesize_s * 1000.0));
    this->period->setText(loc->toString(parameters.period_s * 1000.0));
    if(parameters.resample_rate_hz > 0) {
        this->resample_rate->setText(loc->toString(static_cast<int>(parameters.resample_rate_hz)));
        this->resampled->setChecked(true);
    }
    this->checkParameters();
}


AudioReaderParameters AudioReaderParametersWidget::getParameters() {
    AudioReaderParameters parameters;
    parameters.period_s = this->period->text().toDouble() / 1000.0;
    parameters.windowstimesize_s = this->windows_size->text().toDouble() / 1000.0;
    if(this->resampled->isChecked()) {
        parameters.resample_rate_hz = this->resample_rate->text().toInt();
    } else {
        parameters.resample_rate_hz = -1;
    }
    return parameters;
}

void AudioReaderParametersWidget::enableResampling(int state) {
    if(state == Qt::Checked) {
        this->resample_rate->setEnabled(true);
    } else {
        this->resample_rate->setDisabled(true);
    }
}


void AudioReaderParametersWidget::checkParameters() {
    int pos = 0;
    QString text;
    std::vector<QLineEdit*> lines{  this->windows_size,
                                    this->period};
    if(this->resampled->isChecked()) {
        lines.push_back(this->resample_rate);
    }
    for(auto const & line: lines) {
        text = line->text();
        if(line->validator()->validate(text, pos) != QValidator::Acceptable) {
            this->ready = false;
            emit this->parametersNotReady();
            return;
        }
    }
    this->ready = true;
    emit this->parametersReady();
}

