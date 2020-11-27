#include "RhythmParametersWidget.hpp"
#include "RhythmDetector.hpp"
#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>
#include <QScrollArea>
#include <Qt>
#include <QDoubleValidator>
#include <algorithm>
#include <QString>


RhythmParametersWidget::RhythmParametersWidget(QWidget *parent/*=nullptr*/): QGroupBox("Rhythm parameters", parent) {
    /* Widgets */
    this->bpm_min = new QLineEdit;
    this->bpm_max = new QLineEdit;
    this->error_max = new QLineEdit;
    this->max_delay_var = new QLineEdit;
    this->OneNoteOneBeat = new QCheckBox("1 note - 1 beat");
    this->OneNoteTwoBeat = new QCheckBox("1 note - 2 beat");
    this->OneNoteThreeBeat = new QCheckBox("1 note - 3 beat");
    this->OneNoteFourBeat = new QCheckBox("1 note - 4 beat");
    this->OneNoteFiveBeat = new QCheckBox("1 note - 5 beat");
    this->OneNoteSixBeat = new QCheckBox("1 note - 6 beat");
    this->OneNoteSevenBeat = new QCheckBox("1 note - 7 beat");
    this->OneNoteEightBeat = new QCheckBox("1 note - 8 beat");
    this->OneRestOneBeat = new QCheckBox("1 rest - 1 beat");
    this->OneRestTwoBeat = new QCheckBox("1 rest - 2 beat");
    this->OneRestThreeBeat = new QCheckBox("1 rest - 3 beat");
    this->OneRestFourBeat = new QCheckBox("1 rest - 4 beat");
    this->OneRestFiveBeat = new QCheckBox("1 rest - 5 beat");
    this->OneRestSixBeat = new QCheckBox("1 rest - 6 beat");
    this->OneRestSevenBeat = new QCheckBox("1 rest - 7 beat");
    this->OneRestEightBeat = new QCheckBox("1 rest - 8 beat");
    this->EN_EN = new QCheckBox("EN_EN");
    this->ER_EN = new QCheckBox("ER_EN");
    this->EN_ER = new QCheckBox("EN_ER");
    this->DEN_SN = new QCheckBox("DEN_SN");
    this->SN_DEN = new QCheckBox("SN_DEN");
    this->DQN_EN = new QCheckBox("DQN_EN");
    this->QR_ER_EN = new QCheckBox("QR-ER_EN");
    this->DQN_ER = new QCheckBox("DQN_ER");
    this->EN_EN_QN = new QCheckBox("EN_EN-QN");
    this->QN_DQN_EN = new QCheckBox("QN-DQN_EN");
    this->QR_QR_ER_EN = new QCheckBox("QR-QR-ER_EN");
    this->QN_DQN_ER = new QCheckBox("QN-DQN_ER");
    this->EN_EN_HN = new QCheckBox("EN_EN-HN");
    this->HN_DQN_EN = new QCheckBox("HN-DQN_EN");
    this->QR_QR_QR_ER_EN = new QCheckBox("QR-QR-QR-ER_EN");
    this->HN_DQN_ER = new QCheckBox("HN-DQN_ER");
    this->EN_EN_DHN = new QCheckBox("EN_EN-DHN");
    this->EN_SN_SN = new QCheckBox("EN_SN_SN");
    this->ER_SN_SN = new QCheckBox("ER_SN_SN");
    this->SN_SN_EN = new QCheckBox("SN_SN_EN");
    this->SN_SN_ER = new QCheckBox("SN_SN_ER");
    this->SN_EN_SN = new QCheckBox("SN_EN_SN");
    this->T_EN_EN_EN = new QCheckBox("T_EN_EN_EN");
    this->T_EN_DEN_SN = new QCheckBox("T_EN_DEN_SN");
    this->T_EN_SN_DEN = new QCheckBox("T_EN_SN_DEN");
    this->T_SN_EN_DEN = new QCheckBox("T_SN_EN_DEN");
    this->T_SN_DEN_EN = new QCheckBox("T_SN_DEN_EN");
    this->T_DEN_EN_SN = new QCheckBox("T_DEN_EN_SN");
    this->T_DEN_SN_EN = new QCheckBox("T_DEN_SN_EN");
    this->EN_QN_EN = new QCheckBox("EN_QN_EN");
    this->DQN_SN_SN = new QCheckBox("DQN_SN_SN");
    this->QN_DQN_SN_SN = new QCheckBox("QN-DQN_SN_SN");
    this->HN_DQN_SN_SN = new QCheckBox("HN-DQN_SN_SN");
    this->SN_SN_SN_SN = new QCheckBox("SN_SN_SN_SN");
    /* Widgets (End) */

    /* Architecture */
    QGridLayout *paramLayout = new QGridLayout();
    paramLayout->addWidget(new QLabel("BPM Min:"), 0, 0);
    paramLayout->addWidget(new QLabel("BPM Max:"), 1, 0);
    paramLayout->addWidget(new QLabel("Max BPM variation"), 2, 0);
    paramLayout->addWidget(new QLabel("Error Max"), 3, 0);
    paramLayout->addWidget(this->bpm_min, 0, 1);
    paramLayout->addWidget(this->bpm_max, 1, 1);
    paramLayout->addWidget(this->max_delay_var, 2, 1);
    paramLayout->addWidget(this->error_max, 3, 1);
    paramLayout->addWidget(new QLabel("[BPM]"), 0, 2);
    paramLayout->addWidget(new QLabel("[BPM]"), 1, 2);
    paramLayout->addWidget(new QLabel("[%]"), 2, 2);
    paramLayout->addWidget(new QLabel("[-]"), 3, 2);


    QGroupBox *OneNoteParam = new QGroupBox("Combinations of 1 note/rest");
    QFormLayout *layoutOneNote = new QFormLayout;
    layoutOneNote->addWidget(this->OneNoteOneBeat);
    layoutOneNote->addWidget(this->OneNoteTwoBeat);
    layoutOneNote->addWidget(this->OneNoteThreeBeat);
    layoutOneNote->addWidget(this->OneNoteFourBeat);
    layoutOneNote->addWidget(this->OneNoteFiveBeat);
    layoutOneNote->addWidget(this->OneNoteSixBeat);
    layoutOneNote->addWidget(this->OneNoteSevenBeat);
    layoutOneNote->addWidget(this->OneNoteEightBeat);
    layoutOneNote->addWidget(this->OneRestOneBeat);
    layoutOneNote->addWidget(this->OneRestTwoBeat);
    layoutOneNote->addWidget(this->OneRestThreeBeat);
    layoutOneNote->addWidget(this->OneRestFourBeat);
    layoutOneNote->addWidget(this->OneRestFiveBeat);
    layoutOneNote->addWidget(this->OneRestSixBeat);
    layoutOneNote->addWidget(this->OneRestSevenBeat);
    layoutOneNote->addWidget(this->OneRestEightBeat);
    OneNoteParam->setLayout(layoutOneNote);

    QGroupBox *TwoNoteParam = new QGroupBox("Combinations of 2 notes/rests");
    QFormLayout *layoutTwoNote = new QFormLayout;
    layoutTwoNote->addWidget(this->EN_EN);
    layoutTwoNote->addWidget(this->ER_EN);
    layoutTwoNote->addWidget(this->EN_ER);
    layoutTwoNote->addWidget(this->DEN_SN);
    layoutTwoNote->addWidget(this->SN_DEN);
    layoutTwoNote->addWidget(this->DQN_EN);
    layoutTwoNote->addWidget(this->QR_ER_EN);
    layoutTwoNote->addWidget(this->DQN_ER);
    layoutTwoNote->addWidget(this->EN_EN_QN);
    layoutTwoNote->addWidget(this->QN_DQN_EN);
    layoutTwoNote->addWidget(this->QR_QR_ER_EN);
    layoutTwoNote->addWidget(this->QN_DQN_ER);
    layoutTwoNote->addWidget(this->EN_EN_HN);
    layoutTwoNote->addWidget(this->HN_DQN_EN);
    layoutTwoNote->addWidget(this->QR_QR_QR_ER_EN);
    layoutTwoNote->addWidget(this->HN_DQN_ER);
    layoutTwoNote->addWidget(this->EN_EN_DHN);
    TwoNoteParam->setLayout(layoutTwoNote);

    QGroupBox *ThreeNoteParam = new QGroupBox("Combinations of 3 notes/rests");
    QFormLayout *layoutThreeNote = new QFormLayout;
    layoutThreeNote->addWidget(this->EN_SN_SN);
    layoutThreeNote->addWidget(this->ER_SN_SN);
    layoutThreeNote->addWidget(this->SN_SN_EN);
    layoutThreeNote->addWidget(this->SN_SN_ER);
    layoutThreeNote->addWidget(this->SN_EN_SN);
    layoutThreeNote->addWidget(this->T_EN_EN_EN);
    layoutThreeNote->addWidget(this->T_EN_DEN_SN);
    layoutThreeNote->addWidget(this->T_EN_SN_DEN);
    layoutThreeNote->addWidget(this->T_SN_EN_DEN);
    layoutThreeNote->addWidget(this->T_SN_DEN_EN);
    layoutThreeNote->addWidget(this->T_DEN_EN_SN);
    layoutThreeNote->addWidget(this->T_DEN_SN_EN);
    layoutThreeNote->addWidget(this->EN_QN_EN);
    layoutThreeNote->addWidget(this->DQN_SN_SN);
    layoutThreeNote->addWidget(this->QN_DQN_SN_SN);
    layoutThreeNote->addWidget(this->HN_DQN_SN_SN);
    ThreeNoteParam->setLayout(layoutThreeNote);

    QGroupBox *FourNoteParam = new QGroupBox("Combinations of 4 notes/rests");
    QFormLayout *layoutFourNote = new QFormLayout;
    layoutFourNote->addWidget(this->SN_SN_SN_SN);
    FourNoteParam->setLayout(layoutFourNote);
    
    QVBoxLayout *combLayout = new QVBoxLayout;
    combLayout->addWidget(OneNoteParam);
    combLayout->addWidget(TwoNoteParam);
    combLayout->addWidget(ThreeNoteParam);
    combLayout->addWidget(FourNoteParam);
    QWidget *combsWidgets = new QWidget;
    combsWidgets->setLayout(combLayout);
    
    QScrollArea *scrollCombs = new QScrollArea;
    scrollCombs->setWidget(combsWidgets);
    scrollCombs->setWidgetResizable(true);
    scrollCombs->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(paramLayout);
    mainLayout->addWidget(scrollCombs);
    /* Architecture (End) */

    /* Design */
    /* Design (End) */

    /* Logic */
    this->ready = false;
    this->bpm_min->setValidator(new QDoubleValidator(0.0, 200.0, 2));         // BPM
    this->bpm_max->setValidator(new QDoubleValidator(0.0, 200.0, 2));         // BPM
    this->error_max->setValidator(new QDoubleValidator(0.0, 100.0, 4));       // -
    this->max_delay_var->setValidator(new QDoubleValidator(0.0, 1000.0, 1));  // -
    /* Logic (End) */

    /* Connexions */
    connect(this->bpm_min, SIGNAL (textEdited(QString)), this, SLOT (checkParameters()));
    connect(this->bpm_max, SIGNAL (textEdited(QString)), this, SLOT (checkParameters()));
    connect(this->error_max, SIGNAL (textEdited(QString)), this, SLOT (checkParameters()));
    connect(this->max_delay_var, SIGNAL (textEdited(QString)), this, SLOT (checkParameters()));
    connect(this->OneNoteOneBeat , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->OneNoteTwoBeat , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->OneNoteThreeBeat , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->OneNoteFourBeat , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->OneNoteFiveBeat , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->OneNoteSixBeat , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->OneNoteSevenBeat , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->OneNoteEightBeat , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->OneRestOneBeat , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->OneRestTwoBeat , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->OneRestThreeBeat , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->OneRestFourBeat , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->OneRestFiveBeat , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->OneRestSixBeat , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->OneRestSevenBeat , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->OneRestEightBeat , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->EN_EN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->ER_EN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->EN_ER , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->DEN_SN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->SN_DEN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->DQN_EN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->QR_ER_EN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->DQN_ER , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->EN_EN_QN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->QN_DQN_EN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->QR_QR_ER_EN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->QN_DQN_ER , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->EN_EN_HN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->HN_DQN_EN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->QR_QR_QR_ER_EN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->HN_DQN_ER , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->EN_EN_DHN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->EN_SN_SN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->ER_SN_SN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->SN_SN_EN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->SN_SN_ER , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->SN_EN_SN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->T_EN_EN_EN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->T_EN_DEN_SN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->T_EN_SN_DEN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->T_SN_EN_DEN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->T_SN_DEN_EN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->T_DEN_EN_SN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->T_DEN_SN_EN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->EN_QN_EN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->DQN_SN_SN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->QN_DQN_SN_SN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->HN_DQN_SN_SN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    connect(this->SN_SN_SN_SN , SIGNAL (stateChanged(int)), this, SLOT (checkParameters()));
    /* Connexions (End) */
}


RhythmParametersWidget::~RhythmParametersWidget() {

}


void RhythmParametersWidget::setDefaultParameters() {
    this->setParameters(DEFAULT_RHYTHM_PARAMETERS);
}


bool RhythmParametersWidget::isReady() {
    return this->ready;
}


RhythmParameters RhythmParametersWidget::getParameters() {
    RhythmParameters parameters;
    parameters.delay_max_s = 60.0 / this->bpm_min->text().toDouble();
    parameters.delay_min_s = 60.0 / this->bpm_max->text().toDouble();
    parameters.error_max = this->error_max->text().toDouble();
    parameters.max_delay_var = this->max_delay_var->text().toDouble() / 100.0;
    parameters.combinations_masked.clear();
    if(!this->OneNoteOneBeat->isChecked()) {parameters.combinations_masked.push_back("1NOTE_1BEAT");};
    if(!this->OneNoteTwoBeat->isChecked()) {parameters.combinations_masked.push_back("1NOTE_2BEATS");};
    if(!this->OneNoteThreeBeat->isChecked()) {parameters.combinations_masked.push_back("1NOTE_3BEATS");};
    if(!this->OneNoteFourBeat->isChecked()) {parameters.combinations_masked.push_back("1NOTE_4BEATS");};
    if(!this->OneNoteFiveBeat->isChecked()) {parameters.combinations_masked.push_back("1NOTE_5BEATS");};
    if(!this->OneNoteSixBeat->isChecked()) {parameters.combinations_masked.push_back("1NOTE_6BEATS");};
    if(!this->OneNoteSevenBeat->isChecked()) {parameters.combinations_masked.push_back("1NOTE_7BEATS");};
    if(!this->OneNoteEightBeat->isChecked()) {parameters.combinations_masked.push_back("1NOTE_8BEATS");};
    if(!this->OneRestOneBeat->isChecked()) {parameters.combinations_masked.push_back("1REST_1BEAT");};
    if(!this->OneRestTwoBeat->isChecked()) {parameters.combinations_masked.push_back("1REST_2BEATS");};
    if(!this->OneRestThreeBeat->isChecked()) {parameters.combinations_masked.push_back("1REST_3BEATS");};
    if(!this->OneRestFourBeat->isChecked()) {parameters.combinations_masked.push_back("1REST_4BEATS");};
    if(!this->OneRestFiveBeat->isChecked()) {parameters.combinations_masked.push_back("1REST_5BEATS");};
    if(!this->OneRestSixBeat->isChecked()) {parameters.combinations_masked.push_back("1REST_6BEATS");};
    if(!this->OneRestSevenBeat->isChecked()) {parameters.combinations_masked.push_back("1REST_7BEATS");};
    if(!this->OneRestEightBeat->isChecked()) {parameters.combinations_masked.push_back("1REST_8BEATS");};
    if(!this->EN_EN->isChecked()) {parameters.combinations_masked.push_back("EN_EN");};
    if(!this->ER_EN->isChecked()) {parameters.combinations_masked.push_back("ER_EN");};
    if(!this->EN_ER->isChecked()) {parameters.combinations_masked.push_back("EN_ER");};
    if(!this->DEN_SN->isChecked()) {parameters.combinations_masked.push_back("DEN_SN");};
    if(!this->SN_DEN->isChecked()) {parameters.combinations_masked.push_back("SN_DEN");};
    if(!this->DQN_EN->isChecked()) {parameters.combinations_masked.push_back("DQN_EN");};
    if(!this->QR_ER_EN->isChecked()) {parameters.combinations_masked.push_back("QR-ER_EN");};
    if(!this->DQN_ER->isChecked()) {parameters.combinations_masked.push_back("DQN_ER");};
    if(!this->EN_EN_QN->isChecked()) {parameters.combinations_masked.push_back("EN_EN-QN");};
    if(!this->QN_DQN_EN->isChecked()) {parameters.combinations_masked.push_back("QN-DQN_EN");};
    if(!this->QR_QR_ER_EN->isChecked()) {parameters.combinations_masked.push_back("QR-QR-ER_EN");};
    if(!this->QN_DQN_ER->isChecked()) {parameters.combinations_masked.push_back("QN-DQN_ER");};
    if(!this->EN_EN_HN->isChecked()) {parameters.combinations_masked.push_back("EN_EN-HN");};
    if(!this->HN_DQN_EN->isChecked()) {parameters.combinations_masked.push_back("HN-DQN_EN");};
    if(!this->QR_QR_QR_ER_EN->isChecked()) {parameters.combinations_masked.push_back("QR-QR-QR-ER_EN");};
    if(!this->HN_DQN_ER->isChecked()) {parameters.combinations_masked.push_back("HN-DQN_ER");};
    if(!this->EN_EN_DHN->isChecked()) {parameters.combinations_masked.push_back("EN_EN-DHN");};
    if(!this->EN_SN_SN->isChecked()) {parameters.combinations_masked.push_back("EN_SN_SN");};
    if(!this->ER_SN_SN->isChecked()) {parameters.combinations_masked.push_back("ER_SN_SN");};
    if(!this->SN_SN_EN->isChecked()) {parameters.combinations_masked.push_back("SN_SN_EN");};
    if(!this->SN_SN_ER->isChecked()) {parameters.combinations_masked.push_back("SN_SN_ER");};
    if(!this->SN_EN_SN->isChecked()) {parameters.combinations_masked.push_back("SN_EN_SN");};
    if(!this->T_EN_EN_EN->isChecked()) {parameters.combinations_masked.push_back("T_EN_EN_EN");};
    if(!this->T_EN_DEN_SN->isChecked()) {parameters.combinations_masked.push_back("T_EN_DEN_SN");};
    if(!this->T_EN_SN_DEN->isChecked()) {parameters.combinations_masked.push_back("T_EN_SN_DEN");};
    if(!this->T_SN_EN_DEN->isChecked()) {parameters.combinations_masked.push_back("T_SN_EN_DEN");};
    if(!this->T_SN_DEN_EN->isChecked()) {parameters.combinations_masked.push_back("T_SN_DEN_EN");};
    if(!this->T_DEN_EN_SN->isChecked()) {parameters.combinations_masked.push_back("T_DEN_EN_SN");};
    if(!this->T_DEN_SN_EN->isChecked()) {parameters.combinations_masked.push_back("T_DEN_SN_EN");};
    if(!this->EN_QN_EN->isChecked()) {parameters.combinations_masked.push_back("EN_QN_EN");};
    if(!this->DQN_SN_SN->isChecked()) {parameters.combinations_masked.push_back("DQN_SN_SN");};
    if(!this->QN_DQN_SN_SN->isChecked()) {parameters.combinations_masked.push_back("QN-DQN_SN_SN");};
    if(!this->HN_DQN_SN_SN->isChecked()) {parameters.combinations_masked.push_back("HN-DQN_SN_SN");};
    if(!this->SN_SN_SN_SN->isChecked()) {parameters.combinations_masked.push_back("SN_SN_SN_SN");};
    return parameters;
}

bool RhythmParametersWidget::contains(const std::vector<std::string> & list, const std::string & element) const {
    return (std::find(list.begin(), list.end(), element) != list.end());
}


void RhythmParametersWidget::setParameters(const RhythmParameters & parameters) {
    QLocale *loc = new QLocale();
    this->bpm_min->setText(loc->toString(60.0 / parameters.delay_max_s));
    this->bpm_max->setText(loc->toString(60.0 / parameters.delay_min_s));
    this->error_max->setText(loc->toString(parameters.error_max));
    this->max_delay_var->setText(loc->toString(parameters.max_delay_var * 100.0));
    this->OneNoteOneBeat->setChecked(!this->contains(parameters.combinations_masked, "1NOTE_1BEAT"));
    this->OneNoteTwoBeat->setChecked(!this->contains(parameters.combinations_masked, "1NOTE_2BEATS"));
    this->OneNoteThreeBeat->setChecked(!this->contains(parameters.combinations_masked, "1NOTE_3BEATS"));
    this->OneNoteFourBeat->setChecked(!this->contains(parameters.combinations_masked, "1NOTE_4BEATS"));
    this->OneNoteFiveBeat->setChecked(!this->contains(parameters.combinations_masked, "1NOTE_5BEATS"));
    this->OneNoteSixBeat->setChecked(!this->contains(parameters.combinations_masked, "1NOTE_6BEATS"));
    this->OneNoteSevenBeat->setChecked(!this->contains(parameters.combinations_masked, "1NOTE_7BEATS"));
    this->OneNoteEightBeat->setChecked(!this->contains(parameters.combinations_masked, "1NOTE_8BEATS"));
    this->OneRestOneBeat->setChecked(!this->contains(parameters.combinations_masked, "1REST_1BEAT"));
    this->OneRestTwoBeat->setChecked(!this->contains(parameters.combinations_masked, "1REST_2BEATS"));
    this->OneRestThreeBeat->setChecked(!this->contains(parameters.combinations_masked, "1REST_3BEATS"));
    this->OneRestFourBeat->setChecked(!this->contains(parameters.combinations_masked, "1REST_4BEATS"));
    this->OneRestFiveBeat->setChecked(!this->contains(parameters.combinations_masked, "1REST_5BEATS"));
    this->OneRestSixBeat->setChecked(!this->contains(parameters.combinations_masked, "1REST_6BEATS"));
    this->OneRestSevenBeat->setChecked(!this->contains(parameters.combinations_masked, "1REST_7BEATS"));
    this->OneRestEightBeat->setChecked(!this->contains(parameters.combinations_masked, "1REST_8BEATS"));
    this->EN_EN->setChecked(!this->contains(parameters.combinations_masked, "EN_EN"));
    this->ER_EN->setChecked(!this->contains(parameters.combinations_masked, "ER_EN"));
    this->EN_ER->setChecked(!this->contains(parameters.combinations_masked, "EN_ER"));
    this->DEN_SN->setChecked(!this->contains(parameters.combinations_masked, "DEN_SN"));
    this->SN_DEN->setChecked(!this->contains(parameters.combinations_masked, "SN_DEN"));
    this->DQN_EN->setChecked(!this->contains(parameters.combinations_masked, "DQN_EN"));
    this->QR_ER_EN->setChecked(!this->contains(parameters.combinations_masked, "QR-ER_EN"));
    this->DQN_ER->setChecked(!this->contains(parameters.combinations_masked, "DQN_ER"));
    this->EN_EN_QN->setChecked(!this->contains(parameters.combinations_masked, "EN_EN-QN"));
    this->QN_DQN_EN->setChecked(!this->contains(parameters.combinations_masked, "QN-DQN_EN"));
    this->QR_QR_ER_EN->setChecked(!this->contains(parameters.combinations_masked, "QR-QR-ER_EN"));
    this->QN_DQN_ER->setChecked(!this->contains(parameters.combinations_masked, "QN-DQN_ER"));
    this->EN_EN_HN->setChecked(!this->contains(parameters.combinations_masked, "EN_EN-HN"));
    this->HN_DQN_EN->setChecked(!this->contains(parameters.combinations_masked, "HN-DQN_EN"));
    this->QR_QR_QR_ER_EN->setChecked(!this->contains(parameters.combinations_masked, "QR-QR-QR-ER_EN"));
    this->HN_DQN_ER->setChecked(!this->contains(parameters.combinations_masked, "HN-DQN_ER"));
    this->EN_EN_DHN->setChecked(!this->contains(parameters.combinations_masked, "EN_EN-DHN"));
    this->EN_SN_SN->setChecked(!this->contains(parameters.combinations_masked, "EN_SN_SN"));
    this->ER_SN_SN->setChecked(!this->contains(parameters.combinations_masked, "ER_SN_SN"));
    this->SN_SN_EN->setChecked(!this->contains(parameters.combinations_masked, "SN_SN_EN"));
    this->SN_SN_ER->setChecked(!this->contains(parameters.combinations_masked, "SN_SN_ER"));
    this->SN_EN_SN->setChecked(!this->contains(parameters.combinations_masked, "SN_EN_SN"));
    this->T_EN_EN_EN->setChecked(!this->contains(parameters.combinations_masked, "T_EN_EN_EN"));
    this->T_EN_DEN_SN->setChecked(!this->contains(parameters.combinations_masked, "T_EN_DEN_SN"));
    this->T_EN_SN_DEN->setChecked(!this->contains(parameters.combinations_masked, "T_EN_SN_DEN"));
    this->T_SN_EN_DEN->setChecked(!this->contains(parameters.combinations_masked, "T_SN_EN_DEN"));
    this->T_SN_DEN_EN->setChecked(!this->contains(parameters.combinations_masked, "T_SN_DEN_EN"));
    this->T_DEN_EN_SN->setChecked(!this->contains(parameters.combinations_masked, "T_DEN_EN_SN"));
    this->T_DEN_SN_EN->setChecked(!this->contains(parameters.combinations_masked, "T_DEN_SN_EN"));
    this->EN_QN_EN->setChecked(!this->contains(parameters.combinations_masked, "EN_QN_EN"));
    this->DQN_SN_SN->setChecked(!this->contains(parameters.combinations_masked, "DQN_SN_SN"));
    this->QN_DQN_SN_SN->setChecked(!this->contains(parameters.combinations_masked, "QN-DQN_SN_SN"));
    this->HN_DQN_SN_SN->setChecked(!this->contains(parameters.combinations_masked, "HN-DQN_SN_SN"));
    this->SN_SN_SN_SN->setChecked(!this->contains(parameters.combinations_masked, "SN_SN_SN_SN"));
    this->checkParameters();
}


void RhythmParametersWidget::checkParameters() {
    int pos = 0;
    QString text;
    std::vector<QLineEdit*> lines{  this->bpm_min,
                                    this->bpm_max,
                                    this->error_max,
                                    this->max_delay_var};
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













