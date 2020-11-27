#ifndef TIME_RANGE_PARAMETERS_WIDGET
#define TIME_RANGE_PARAMETERS_WIDGET

#include "AudioReader.hpp"
#include "McLeodPitchExtractorMethod.hpp"
#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>


struct TimeRangeParameters {
    double min_s;
    double max_s;
};

class TimeRangeParametersWidget: public QGroupBox {
    Q_OBJECT
public:
    TimeRangeParametersWidget(QWidget *parent=nullptr);
    ~TimeRangeParametersWidget();
    bool isReady();
    void setValidatorRange(double min, double max);
    TimeRangeParameters getParameters();
private:
    QLineEdit *time_start;
    QLineEdit *time_stop;
    bool ready;
public slots:
    void setParameters(const TimeRangeParameters & parameters);
    void setDefaultParameters();
private slots:
    void checkParameters();
    void enableCropping(bool checked);
signals:
    void parametersReady();
    void parametersNotReady();
};

#endif /* TIME_RANGE_PARAMETERS_WIDGET */
