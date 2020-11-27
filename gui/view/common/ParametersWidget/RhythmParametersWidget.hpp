#ifndef RHYTHM_PARAMETERS_WIDGET
#define RHYTHM_PARAMETERS_WIDGET

#include <QWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include "RhythmDetector.hpp"

class RhythmParametersWidget: public QGroupBox {
    Q_OBJECT
public:
    RhythmParametersWidget(QWidget *parent=nullptr);
    ~RhythmParametersWidget();
    bool isReady();
    RhythmParameters getParameters();
private:
    QLineEdit *bpm_min;
    QLineEdit *bpm_max;
    QLineEdit *error_max;
    QLineEdit *max_delay_var;
    QCheckBox *OneNoteOneBeat;
    QCheckBox *OneNoteTwoBeat;
    QCheckBox *OneNoteThreeBeat;
    QCheckBox *OneNoteFourBeat;
    QCheckBox *OneNoteFiveBeat;
    QCheckBox *OneNoteSixBeat;
    QCheckBox *OneNoteSevenBeat;
    QCheckBox *OneNoteEightBeat;
    QCheckBox *OneRestOneBeat;
    QCheckBox *OneRestTwoBeat;
    QCheckBox *OneRestThreeBeat;
    QCheckBox *OneRestFourBeat;
    QCheckBox *OneRestFiveBeat;
    QCheckBox *OneRestSixBeat;
    QCheckBox *OneRestSevenBeat;
    QCheckBox *OneRestEightBeat;
    QCheckBox *EN_EN;
    QCheckBox *ER_EN;
    QCheckBox *EN_ER;
    QCheckBox *DEN_SN;
    QCheckBox *SN_DEN;
    QCheckBox *DQN_EN;
    QCheckBox *QR_ER_EN;
    QCheckBox *DQN_ER;
    QCheckBox *EN_EN_QN;
    QCheckBox *QN_DQN_EN;
    QCheckBox *QR_QR_ER_EN;
    QCheckBox *QN_DQN_ER;
    QCheckBox *EN_EN_HN;
    QCheckBox *HN_DQN_EN;
    QCheckBox *QR_QR_QR_ER_EN;
    QCheckBox *HN_DQN_ER;
    QCheckBox *EN_EN_DHN;
    QCheckBox *EN_SN_SN;
    QCheckBox *ER_SN_SN;
    QCheckBox *SN_SN_EN;
    QCheckBox *SN_SN_ER;
    QCheckBox *SN_EN_SN;
    QCheckBox *T_EN_EN_EN;
    QCheckBox *T_EN_DEN_SN;
    QCheckBox *T_EN_SN_DEN;
    QCheckBox *T_SN_EN_DEN;
    QCheckBox *T_SN_DEN_EN;
    QCheckBox *T_DEN_EN_SN;
    QCheckBox *T_DEN_SN_EN;
    QCheckBox *EN_QN_EN;
    QCheckBox *DQN_SN_SN;
    QCheckBox *QN_DQN_SN_SN;
    QCheckBox *HN_DQN_SN_SN;
    QCheckBox *SN_SN_SN_SN;
    bool ready;
    bool contains(const std::vector<std::string> & list, const std::string & element) const;
private slots:
    void checkParameters();
public slots:
    void setParameters(const RhythmParameters & parameters);
    void setDefaultParameters();
signals:
    void parametersReady();
    void parametersNotReady();
};

#endif /* RHYTHM_PARAMETERS_WIDGET */
