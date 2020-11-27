#include "TimeRangeParametersWidget.hpp"
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


TimeRangeParametersWidget::TimeRangeParametersWidget(QWidget *parent/*=nullptr*/): QGroupBox("Cropping", parent) {
    /* Widgets */
    this->setCheckable(true);
    this->time_start = new QLineEdit;
    this->time_stop = new QLineEdit;
    QLabel *start_unit = new QLabel("[s]");
    QLabel *stop_unit = new QLabel("[s]");
    /* Widgets (End) */

    /* Architecture */
    this->time_start->setFixedWidth(80);
    this->time_stop->setFixedWidth(80);
    start_unit->setFixedWidth(35);
    stop_unit->setFixedWidth(35);
    QHBoxLayout *start_layout = new QHBoxLayout;
    start_layout->addWidget(new QLabel("Time start:"));
    start_layout->addStretch();
    start_layout->addWidget(this->time_start);
    start_layout->addWidget(start_unit);
    QHBoxLayout *stop_layout = new QHBoxLayout;
    stop_layout->addWidget(new QLabel("Time stop:"));
    stop_layout->addStretch();
    stop_layout->addWidget(this->time_stop);
    stop_layout->addWidget(stop_unit);
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addLayout(start_layout);
    main_layout->addLayout(stop_layout);
    /* Architecture (End) */

    /* Design */
    /* Design (End) */

    /* Logic */
    this->ready = true;
    this->setChecked(false);
    this->time_start->setDisabled(true);
    this->time_stop->setDisabled(true);
    this->time_start->setValidator(new QDoubleValidator(0.0, 1000.0, 3));           // seconds
    this->time_stop->setValidator(new QDoubleValidator(0.0, 1000.0, 3));            // seconds
    /* Logic (End) */

    /* Connexions */
    connect(this, SIGNAL (clicked(bool)), this, SLOT (enableCropping(bool)));
    connect(this, SIGNAL (clicked(bool)), this, SLOT (checkParameters()));
    connect(this->time_start, SIGNAL (textEdited(QString)), this, SLOT (checkParameters()));
    connect(this->time_stop, SIGNAL (textEdited(QString)), this, SLOT (checkParameters()));
    /* Connexions (End) */
}

TimeRangeParametersWidget::~TimeRangeParametersWidget() {

}


void TimeRangeParametersWidget::setDefaultParameters() {
    this->setChecked(false);
    this->time_start->setText("");
    this->time_stop->setText("");
}


bool TimeRangeParametersWidget::isReady() {
    return this->ready;
}


void TimeRangeParametersWidget::setValidatorRange(double min, double max) {
    this->time_start->setValidator(new QDoubleValidator(min, max, 3));
    this->time_stop->setValidator(new QDoubleValidator(min, max, 3));
}


void TimeRangeParametersWidget::setParameters(const TimeRangeParameters & parameters) {
    QLocale *loc = new QLocale();
    this->time_start->setText(loc->toString(parameters.min_s));
    this->time_stop->setText(loc->toString(parameters.max_s));
    this->checkParameters();
}


TimeRangeParameters TimeRangeParametersWidget::getParameters() {
    TimeRangeParameters parameters;
    if(this->isChecked()) {
        parameters.min_s = this->time_start->text().toDouble();
        parameters.max_s = this->time_stop->text().toDouble();
    } else {
        parameters.min_s = -1.0;
        parameters.max_s = -1.0;
    }
    return parameters;
}



void TimeRangeParametersWidget::enableCropping(bool checked) {
    if(checked) {
        this->time_start->setEnabled(true);
        this->time_stop->setEnabled(true);
    } else {
        this->time_start->setDisabled(true);
        this->time_stop->setDisabled(true);
    }
}


void TimeRangeParametersWidget::checkParameters() {
    int pos = 0;
    QString text;
    if(this->isChecked()) {
        for(auto const & line: {this->time_start, this->time_stop}) {
            text = line->text();
            if(line->validator()->validate(text, pos) != QValidator::Acceptable) {
                this->ready = false;
                emit this->parametersNotReady();
                return;
            }
        }
        if(this->time_start->text().toDouble() >= this->time_stop->text().toDouble()) {
            this->ready = false;
            emit this->parametersNotReady();
            return;
        }
    }
    this->ready = true;
    emit this->parametersReady();
}

