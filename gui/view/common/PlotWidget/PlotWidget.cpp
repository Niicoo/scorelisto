#include "PlotWidget.hpp"
#include "PitchDetector.hpp"
#include "StepDetector.hpp"
#include "common_tools.hpp"
#include <QWidget>
#include <QString>
#include <QtCharts>
#include <QRectF>
#include <QVector>
#include <thread>
#include <math.h>

enum ENERGY_UNIT{DB=0, NAT=1};
enum PITCH_UNIT{ST=0, HZ=1};


PlotWidget::PlotWidget(QWidget *parent): QWidget(parent) {
    /* Widgets */
    this->serie_pitch_raw = new QLineSeries();
    this->serie_pitch_quantized = new QLineSeries();
    this->serie_energy = new QLineSeries();
    this->check_raw = new QCheckBox("Raw pitch", this);
    this->check_quantized = new QCheckBox("Quantized pitch", this);
    this->check_energy = new QCheckBox("Energy", this);
    this->energy_unit = new QComboBox(this);
    this->pitch_unit = new QComboBox(this);
    this->chart_pitch = new QChart();
    this->chart_energy = new QChart();
    this->chartview_pitch = new QChartView(this->chart_pitch, this);
    this->chartview_energy = new QChartView(this->chart_energy, this);
    this->axisX_pitch = new QValueAxis;
    this->axisX_energy = new QValueAxis;
    this->axisY_pitch = new QValueAxis;
    this->axisY_energy = new QValueAxis;
    /* Widgets (End) */

    /* Logic */
    this->serie_pitch_raw->setName("Raw");
    this->serie_pitch_quantized->setName("Quantized");
    this->serie_energy->setName("Energy");
    this->energy_unit->addItem(QString("Decibel"));
    this->energy_unit->addItem(QString("Natural"));
    this->energy_unit->setCurrentIndex(0);
    this->pitch_unit->addItem(QString("Semitone"));
    this->pitch_unit->addItem(QString("Hertz"));
    this->pitch_unit->setCurrentIndex(0);
    this->check_raw->setChecked(true);
    this->check_quantized->setChecked(true);
    this->check_energy->setChecked(true);
    this->chart_energy->legend()->hide();
    this->chart_pitch->addSeries(this->serie_pitch_raw);
    this->chart_pitch->addSeries(this->serie_pitch_quantized);
    this->chart_energy->addSeries(this->serie_energy);
    this->chart_energy->setTitle("Energy");
    this->chart_pitch->setTitle("Pitch");
    this->chartview_pitch->setRenderHint(QPainter::Antialiasing);
    this->chartview_energy->setRenderHint(QPainter::Antialiasing);
    this->chartview_pitch->setRubberBand(QChartView::RectangleRubberBand);
    this->chartview_energy->setRubberBand(QChartView::RectangleRubberBand);
    this->axisX_pitch->setTitleText("Time [s]");
    this->axisX_energy->setTitleText("Time [s]");
    this->axisY_pitch->setTitleText("Pitch [semitone]");
    this->axisY_energy->setTitleText("Pitch [dB]");
    this->chart_pitch->addAxis(this->axisX_pitch, Qt::AlignBottom);
    this->chart_pitch->addAxis(this->axisY_pitch, Qt::AlignLeft);
    this->chart_energy->addAxis(this->axisX_energy, Qt::AlignBottom);
    this->chart_energy->addAxis(this->axisY_energy, Qt::AlignLeft);
    this->serie_pitch_raw->attachAxis(this->axisX_pitch);
    this->serie_pitch_raw->attachAxis(this->axisY_pitch);
    this->serie_pitch_quantized->attachAxis(this->axisX_pitch);
    this->serie_pitch_quantized->attachAxis(this->axisY_pitch);
    this->serie_energy->attachAxis(this->axisX_energy);
    this->serie_energy->attachAxis(this->axisY_energy);
    /* Logic (End) */

    /* Design */
    /* Design (End) */

    /* Architecture */
    QHBoxLayout *child1_layout = new QHBoxLayout;
    child1_layout->addWidget(this->check_raw);
    child1_layout->addWidget(this->check_quantized);
    child1_layout->addWidget(this->pitch_unit);
    QHBoxLayout *child2_layout = new QHBoxLayout;
    child2_layout->addWidget(this->check_energy);
    child2_layout->addWidget(this->energy_unit);
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addLayout(child1_layout);
    main_layout->addLayout(child2_layout);
    main_layout->addWidget(this->chartview_pitch);
    main_layout->addWidget(this->chartview_energy);
    /* Architecture (End) */

    /* Connexions */
    connect(this->check_raw, SIGNAL (stateChanged(int)), this, SLOT (showRawPlot(int)));
    connect(this->check_quantized, SIGNAL (stateChanged(int)), this, SLOT (showQuantizedPlot(int)));
    connect(this->check_energy, SIGNAL (stateChanged(int)), this, SLOT (showEnergyPlot(int)));
    connect(this->energy_unit, SIGNAL (currentIndexChanged(int)), this, SLOT (updateEnergyUnit(int)));
    connect(this->pitch_unit, SIGNAL (currentIndexChanged(int)), this, SLOT (updatePitchUnit(int)));
    connect(this->axisX_pitch, SIGNAL (rangeChanged(qreal, qreal)), this, SLOT (setEnergyRange(qreal, qreal)));
    connect(this->axisX_energy, SIGNAL (rangeChanged(qreal, qreal)), this, SLOT (setPitchRange(qreal, qreal)));
    /* Connexions (End) */
}

PlotWidget::~PlotWidget() {

}


bool PlotWidget::isPitchResultsPlotted() {
    return (this->serie_pitch_raw->count() != 0);
}


bool PlotWidget::isStepResultsPlotted() {
    return (this->serie_pitch_quantized->count() != 0);
}


void PlotWidget::showRawPlot(int state) {
    if((state == Qt::Checked)){
        this->chart_pitch->addSeries(this->serie_pitch_raw);
        this->chartview_pitch->show();
    } else {
        if(!this->check_quantized->isChecked()) {
            this->chartview_pitch->hide();
        }
        this->chart_pitch->removeSeries(this->serie_pitch_raw);
    }
}

void PlotWidget::showQuantizedPlot(int state) {
    if((state == Qt::Checked)){
        this->chartview_pitch->show();
        this->chart_pitch->addSeries(this->serie_pitch_quantized);
    } else {
        if(!this->check_raw->isChecked()) {
            this->chartview_pitch->hide();
        }
        this->chart_pitch->removeSeries(this->serie_pitch_quantized);
    }
}

void PlotWidget::showEnergyPlot(int state) {
    if(state == Qt::Checked) {
        this->chartview_energy->show();
    } else {
        this->chartview_energy->hide();
    }
}

// void PlotWidget::updatePitchResult(const PitchResult & pitch_result) {
//     std::thread(&PlotWidget::updatePitchResultProcess, this, pitch_result).detach();
// }


// void PlotWidget::updateStepResult(const StepResult & step_result) {
//     std::thread(&PlotWidget::updateStepResultProcess, this, step_result).detach();
// }

// void PlotWidget::updatePitchResultProcess(PlotWidget * inst, const PitchResult & pitch_result) {

void PlotWidget::updatePitchResult(const PitchResult & pitch_result) {
    this->energy_unit->setDisabled(true);
    this->pitch_unit->setDisabled(true);
    int unit_energy = this->energy_unit->currentIndex();
    int unit_pitch = this->pitch_unit->currentIndex();
    double t = pitch_result.offset_s;
    double temp_energy;
    double temp_pitch;
    this->t_min = pitch_result.offset_s;
    this->t_max = this->t_min + pitch_result.pitch_st.size() * pitch_result.period_s;
    QVector<QPointF> pitch_buffer;
    QVector<QPointF> energy_buffer;
    pitch_buffer.reserve(pitch_result.pitch_st.size());
    energy_buffer.reserve(pitch_result.pitch_st.size());
    this->pitch_max = -std::numeric_limits<double>::max();
    this->pitch_min = std::numeric_limits<double>::max();
    this->energy_max = -std::numeric_limits<double>::max();
    this->energy_min = std::numeric_limits<double>::max();
    for(size_t k=0; k < pitch_result.pitch_st.size(); k++) {
        temp_energy = (unit_energy == ENERGY_UNIT::NAT) ? pitch_result.energy[k] : 10.0*log10(pitch_result.energy[k]);
        temp_pitch = (unit_pitch == PITCH_UNIT::ST) ? pitch_result.pitch_st[k] : convert_tone_to_freq(pitch_result.pitch_st[k]);
        if(!isnan(temp_pitch)) {
            pitch_buffer.push_back(QPointF(t, temp_pitch));
            if(temp_pitch > this->pitch_max) {this->pitch_max = temp_pitch;}
            if(temp_pitch < this->pitch_min) {this->pitch_min = temp_pitch;}
        } else {
            pitch_buffer.push_back(QPointF(t, 0.0));
        }
        energy_buffer.push_back(QPointF(t, temp_energy));
        if(temp_energy > this->energy_max) {this->energy_max = temp_energy;}
        if(temp_energy < this->energy_min) {this->energy_min = temp_energy;}
        t += pitch_result.period_s;
    }
    this->serie_pitch_quantized->clear();
    this->serie_energy->replace(energy_buffer);
    this->serie_pitch_raw->replace(pitch_buffer);
    this->resetZoom();
    this->energy_unit->setEnabled(true);
    this->pitch_unit->setEnabled(true);
}

// void PlotWidget::updateStepResultProcess(PlotWidget * inst, const StepResult & step_result) {
void PlotWidget::updateStepResult(const StepResult & step_result) {
    this->pitch_unit->setDisabled(true);
    int unit_pitch = this->pitch_unit->currentIndex();
    this->serie_pitch_quantized->clear();
    double t = step_result.offset_s;
    double pitch;
    QVector<QPointF> pitch_buffer;
    pitch_buffer.reserve(step_result.notes.size() * 2);
    for(const auto & note: step_result.notes) {
        if(note.is_a_note) {
            if(unit_pitch == PITCH_UNIT::ST) {
                pitch = note.pitch_st;
            } else {
                pitch = convert_tone_to_freq(note.pitch_st);
            }
        } else {
            pitch = 0.0;
        }
        pitch_buffer.push_back(QPointF(t, pitch));
        pitch_buffer.push_back(QPointF(t + note.length_s, pitch));
        t += note.length_s;
    }
    this->serie_pitch_quantized->replace(pitch_buffer);
    this->pitch_unit->setDisabled(true);
}


void PlotWidget::updatePitchUnit(int index) {
    QVector<QPointF> points = this->serie_pitch_raw->pointsVector();
    qreal min = this->axisY_pitch->min();
    qreal max = this->axisY_pitch->max();
    if(index == 0) {
        this->axisY_pitch->setTitleText("Pitch [semitone]");
        for(int k=0; k < points.size(); k++) {
            if(points[k].y() > 0) {
                points[k].setY(convert_freq_to_tone(points[k].y()));
            }
        }
        min = convert_freq_to_tone(min);
        max = convert_freq_to_tone(max);
    } else {
        this->axisY_pitch->setTitleText("Pitch [hertz]");
        for(int k=0; k < this->serie_pitch_raw->count(); k++) {
            if(points[k].y() > 0) {
                points[k].setY(convert_tone_to_freq(points[k].y()));
            }
        }
        min = convert_tone_to_freq(min);
        max = convert_tone_to_freq(max);
    }
    this->axisY_pitch->setRange(min, max);
    this->serie_pitch_raw->replace(points);
}

void PlotWidget::updateEnergyUnit(int index) {
    QVector<QPointF> points = this->serie_energy->pointsVector();
    qreal min = this->axisY_energy->min();
    qreal max = this->axisY_energy->max();
    if(index == 0) {
        this->axisY_energy->setTitleText("Energy [dB]");
        for(int k=0; k < points.size(); k++) {
            points[k].setY(10.0*log10(points[k].y()));
        }
        min = 10.0*log10(min);
        max = 10.0*log10(max);
    } else {
        this->axisY_energy->setTitleText("Energy [nat]");
        for(int k=0; k < points.size(); k++) {
            points[k].setY(pow(10.0, points[k].y()/10.0));
        }
        min = pow(10.0, min/10.0);
        max = pow(10.0, max/10.0);
    }
    this->axisY_energy->setRange(min, max);
    this->serie_energy->replace(points); 
}


void PlotWidget::clearQuantizedPitch() {
    this->serie_pitch_raw->clear();
    this->serie_energy->clear();
}

void PlotWidget::clearRawPitch() {
    this->serie_pitch_quantized->clear();
}

void PlotWidget::clear() {
    this->serie_pitch_raw->clear();
    this->serie_pitch_quantized->clear();
    this->serie_energy->clear();
}


void PlotWidget::resetZoom() {
    this->axisX_pitch->setRange(this->t_min, this->t_max);
    this->axisX_energy->setRange(this->t_min, this->t_max);
    this->axisY_pitch->setRange(this->pitch_min, this->pitch_max);
    this->axisY_energy->setRange(this->energy_min, this->energy_max);
    // zoomReset()
}


void PlotWidget::setEnergyRange(qreal min, qreal max) {
    bool old_state = this->axisX_energy->blockSignals(true);
    this->axisX_energy->setRange(min, max);
    this->axisX_energy->blockSignals(old_state);
    this->chartview_energy->repaint();
}


void PlotWidget::setPitchRange(qreal min, qreal max) {
    bool old_state = this->axisX_pitch->blockSignals(true);
    this->axisX_pitch->setRange(min, max);
    this->axisX_pitch->blockSignals(old_state);
    this->chartview_pitch->repaint();
}
