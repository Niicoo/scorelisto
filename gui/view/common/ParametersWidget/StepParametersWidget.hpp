#ifndef STEP_PARAMETERS_WIDGET
#define STEP_PARAMETERS_WIDGET

#include <QWidget>
#include <QLineEdit>
#include "StepDetector.hpp"
#include <QGroupBox>

class StepParametersWidget: public QGroupBox {
    Q_OBJECT
public:
    StepParametersWidget(QWidget *parent=nullptr);
    ~StepParametersWidget();
    bool isReady();
    QLineEdit *min_note_length_ms;
    QLineEdit *min_note_gap_st;
    QLineEdit *median_filter_width_ms;
    QLineEdit *min_pitch_st;
    QLineEdit *max_pitch_st;
    bool ready;
    StepParameters getParameters();
private slots:
    void checkParameters();
public slots:
    void setParameters(const StepParameters & parameters);
    void setDefaultParameters();
signals:
    void parametersReady();
    void parametersNotReady();
};

#endif /* STEP_PARAMETERS_WIDGET */
