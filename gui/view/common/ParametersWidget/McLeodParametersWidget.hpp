#ifndef MCLEOD_PARAMETERS_WIDGET
#define MCLEOD_PARAMETERS_WIDGET

#include "AudioReader.hpp"
#include "McLeodPitchExtractorMethod.hpp"
#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>


class McLeodParametersWidget: public QGroupBox {
    Q_OBJECT
public:
    McLeodParametersWidget(QWidget *parent=nullptr);
    ~McLeodParametersWidget();
    bool isReady();
    McLeodParameters getParameters();
private:
    QLineEdit *cutoff;
    QLineEdit *small_cutoff;
    QLineEdit *lower_pitch_cutoff;
    bool ready;
private slots:
    void checkParameters();
public slots:
    void setParameters(const McLeodParameters & parameters);
    void setDefaultParameters();
signals:
    void parametersReady();
    void parametersNotReady();
};

#endif /* MCLEOD_PARAMETERS_WIDGET */
