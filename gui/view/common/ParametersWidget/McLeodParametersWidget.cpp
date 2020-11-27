#include "McLeodParametersWidget.hpp"
#include "AudioReader.hpp"
#include "McLeodPitchExtractorMethod.hpp"

#include <QWidget>
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QLocale>
#include <QString>


McLeodParametersWidget::McLeodParametersWidget(QWidget *parent/*=nullptr*/): QGroupBox("Mc-Leod Parameters", parent) {
    /* Widgets */
    this->cutoff = new QLineEdit;
    this->small_cutoff = new QLineEdit;
    this->lower_pitch_cutoff = new QLineEdit;
    QLabel* cutoff_unit = new QLabel("[0-1]");
    QLabel* smcutoff_unit = new QLabel("[0-1]");
    QLabel* ptcutoff_unit = new QLabel("[Hz]");
    /* Widgets (End) */

    /* Architecture */
    this->cutoff->setFixedWidth(80);
    this->small_cutoff->setFixedWidth(80);
    this->lower_pitch_cutoff->setFixedWidth(80);
    cutoff_unit->setFixedWidth(35);
    smcutoff_unit->setFixedWidth(35);
    ptcutoff_unit->setFixedWidth(35);
    QHBoxLayout *cutoff_layout = new QHBoxLayout;
    cutoff_layout->addWidget(new QLabel("Cutoff:"));
    cutoff_layout->addStretch();
    cutoff_layout->addWidget(this->cutoff);
    cutoff_layout->addWidget(cutoff_unit);
    QHBoxLayout *smcutoff_layout = new QHBoxLayout;
    smcutoff_layout->addWidget(new QLabel("Small cutoff:"));
    smcutoff_layout->addStretch();
    smcutoff_layout->addWidget(this->small_cutoff);
    smcutoff_layout->addWidget(smcutoff_unit);
    QHBoxLayout *ptcutoff_layout = new QHBoxLayout;
    ptcutoff_layout->addWidget(new QLabel("Minimum frequency:"));
    ptcutoff_layout->addStretch();
    ptcutoff_layout->addWidget(this->lower_pitch_cutoff);
    ptcutoff_layout->addWidget(ptcutoff_unit);
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addLayout(cutoff_layout);
    main_layout->addLayout(smcutoff_layout);
    main_layout->addLayout(ptcutoff_layout);
    /* Architecture (End) */

    /* Logic */
    this->ready = false;
    this->cutoff->setValidator(new QDoubleValidator(0.0, 1.0, 4));                  // [0, 1]
    this->small_cutoff->setValidator(new QDoubleValidator(0.0, 1.0, 4));            // [0, 1]
    this->lower_pitch_cutoff->setValidator(new QDoubleValidator(0.0, 10000.0, 4));  // Hertz
    /* Logic (End) */

    /* Connexions */
    connect(this->cutoff, SIGNAL (textEdited(QString)), this, SLOT (checkParameters()));
    connect(this->small_cutoff, SIGNAL (textEdited(QString)), this, SLOT (checkParameters()));
    connect(this->lower_pitch_cutoff, SIGNAL (textEdited(QString)), this, SLOT (checkParameters()));
    /* Connexions (End) */
}

McLeodParametersWidget::~McLeodParametersWidget() {

}


void McLeodParametersWidget::setDefaultParameters() {
    this->setParameters(DEFAULT_MC_LEOD_PARAMETERS);
}


bool McLeodParametersWidget::isReady() {
    return this->ready;
}


void McLeodParametersWidget::setParameters(const McLeodParameters & parameters) {
    QLocale *loc = new QLocale();
    this->cutoff->setText(loc->toString(parameters.cutoff));
    this->small_cutoff->setText(loc->toString(parameters.small_cutoff));
    this->lower_pitch_cutoff->setText(loc->toString(parameters.lower_pitch_cutoff));
    this->checkParameters();
}


McLeodParameters McLeodParametersWidget::getParameters() {
    McLeodParameters parameters;
    parameters.cutoff = this->cutoff->text().toDouble();
    parameters.small_cutoff = this->small_cutoff->text().toDouble();
    parameters.lower_pitch_cutoff = this->lower_pitch_cutoff->text().toDouble();
    return parameters;
}


void McLeodParametersWidget::checkParameters() {
    int pos = 0;
    QString text;
    std::vector<QLineEdit*> lines{  this->cutoff,
                                    this->small_cutoff,
                                    this->lower_pitch_cutoff};

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

