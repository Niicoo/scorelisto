#include "StepParametersWidget.hpp"
#include "StepDetector.hpp"
#include <QWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QDoubleValidator>
#include <QLineEdit>
#include <QLabel>
#include <QString>


StepParametersWidget::StepParametersWidget(QWidget *parent/*=nullptr*/): QGroupBox("Step parameters", parent) {
    /* Widgets */
    this->min_note_length_ms = new QLineEdit;
    this->min_note_gap_st = new QLineEdit;
    this->median_filter_width_ms = new QLineEdit;
    this->min_pitch_st = new QLineEdit;
    this->max_pitch_st = new QLineEdit;
    /* Widgets (End) */

    /* Architecture */
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(new QLabel("Minimum note length:"), 0, 0);
    layout->addWidget(new QLabel("Minimum gap:"), 1, 0);
    layout->addWidget(new QLabel("Median filter:"), 2, 0);
    layout->addWidget(new QLabel("Pitch min:"), 3, 0);
    layout->addWidget(new QLabel("Pitch max:"), 4, 0);
    layout->addWidget(this->min_note_length_ms, 0, 1);
    layout->addWidget(this->min_note_gap_st, 1, 1);
    layout->addWidget(this->median_filter_width_ms, 2, 1);
    layout->addWidget(this->min_pitch_st, 3, 1);
    layout->addWidget(this->max_pitch_st, 4, 1);
    layout->addWidget(new QLabel("[ms]"), 0, 2);
    layout->addWidget(new QLabel("[semitone]"), 1, 2);
    layout->addWidget(new QLabel("[ms]"), 2, 2);
    layout->addWidget(new QLabel("[semitone]"), 3, 2);
    layout->addWidget(new QLabel("[semitone]"), 4, 2);
    /* Architecture (End) */

    /* Design */
    /* Design (End) */

    /* Logic */
    this->ready = false;
    this->min_note_length_ms->setValidator(new QDoubleValidator(0.0, 1000.0, 4));     // Milliseconds
    this->min_note_gap_st->setValidator(new QDoubleValidator(0, 1, 4));               // Semitone
    this->median_filter_width_ms->setValidator(new QDoubleValidator(0.0, 1000.0, 4)); // Milliseconds
    this->min_pitch_st->setValidator(new QDoubleValidator(0.0, 1000.0, 4));              // Semitone
    this->max_pitch_st->setValidator(new QDoubleValidator(0.0, 1000.0, 4));          // Semitone
    /* Logic (End) */

    /* Connexions */
    connect(this->min_note_length_ms, SIGNAL (textEdited(QString)), this, SLOT (checkParameters()));
    connect(this->min_note_gap_st, SIGNAL (textEdited(QString)), this, SLOT (checkParameters()));
    connect(this->median_filter_width_ms, SIGNAL (textEdited(QString)), this, SLOT (checkParameters()));
    connect(this->min_pitch_st, SIGNAL (textEdited(QString)), this, SLOT (checkParameters()));
    connect(this->max_pitch_st, SIGNAL (textEdited(QString)), this, SLOT (checkParameters()));
    /* Connexions (End) */
}

StepParametersWidget::~StepParametersWidget() {

}


void StepParametersWidget::setDefaultParameters() {
    this->setParameters(DEFAULT_STEP_PARAMETERS);
}


bool StepParametersWidget::isReady() {
    return this->ready;
}


void StepParametersWidget::setParameters(const StepParameters & parameters) {
    QLocale *loc = new QLocale();
    this->min_note_length_ms->setText(loc->toString(parameters.min_note_length_s * 1000.0));
    this->min_note_gap_st->setText(loc->toString(parameters.min_note_gap_st, 'g', 3));
    this->median_filter_width_ms->setText(loc->toString(parameters.median_filter_width_s * 1000.0));
    this->min_pitch_st->setText(loc->toString(parameters.min_pitch_st));
    this->max_pitch_st->setText(loc->toString(parameters.max_pitch_st));
    this->checkParameters();
}


StepParameters StepParametersWidget::getParameters() {
    StepParameters parameters;
    parameters.min_note_length_s = this->min_note_length_ms->text().toDouble() / 1000.0;
    parameters.min_note_gap_st = this->min_note_gap_st->text().toDouble();
    parameters.median_filter_width_s = this->median_filter_width_ms->text().toDouble() / 1000.0;
    parameters.min_pitch_st = this->min_pitch_st->text().toDouble();
    parameters.max_pitch_st = this->max_pitch_st->text().toDouble();
    return parameters;
}


void StepParametersWidget::checkParameters() {
    int pos = 0;
    QString text;
    std::vector<QLineEdit*> lines{  this->min_note_length_ms,
                                    this->min_note_gap_st,
                                    this->median_filter_width_ms,
                                    this->min_pitch_st,
                                    this->max_pitch_st};
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
