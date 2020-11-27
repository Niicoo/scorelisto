#ifndef PLOT_WIDGET
#define PLOT_WIDGET

#include "PitchDetector.hpp"
#include "StepDetector.hpp"
#include <QWidget>
#include <QCheckBox>
#include <QtCharts>
#include <QRectF>

class PlotWidget: public QWidget {
    Q_OBJECT
public:
    PlotWidget(QWidget *parent = nullptr);
    ~PlotWidget();
    bool isPitchResultsPlotted();
    bool isStepResultsPlotted();
private:
    QLineSeries *serie_pitch_raw;
    QLineSeries *serie_pitch_quantized;
    QLineSeries *serie_energy;
    QCheckBox *check_raw;
    QCheckBox *check_quantized;
    QCheckBox *check_energy;
    QComboBox *energy_unit;
    QComboBox *pitch_unit;
    QChartView *chartview_pitch;
    QChartView *chartview_energy;
    QChart *chart_pitch;
    QChart *chart_energy;
    QValueAxis *axisY_pitch;
    QValueAxis *axisY_energy;
    QValueAxis *axisX_pitch;
    QValueAxis *axisX_energy;
    double energy_max;
    double energy_min;
    double pitch_min;
    double pitch_max;
    double t_min;
    double t_max;
    // static void updatePitchResultProcess(PlotWidget * inst, const PitchResult & pitch_result);
    // static void updateStepResultProcess(PlotWidget * inst, const StepResult & step_result);
private slots:
    void resetZoom();
public slots:
    void showRawPlot(int state);
    void showQuantizedPlot(int state);
    void showEnergyPlot(int state);

    void updatePitchResult(const PitchResult & pitch_result);
    void updateStepResult(const StepResult & step_result);

    void updatePitchUnit(int index);
    void updateEnergyUnit(int index);
    void clearQuantizedPitch();
    void clearRawPitch();
    void clear();
    void setEnergyRange(qreal min, qreal max);
    void setPitchRange(qreal min, qreal max);
};

#endif /* PLOT_WIDGET */
