#include "StreamSelectorWidget.hpp"
#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QString>


StreamSelectorWidget::StreamSelectorWidget(QWidget *parent/*=nullptr*/): QWidget(parent) {
    /* Widgets */
    QLabel *label_combo = new QLabel("Stream index", this);
    this->combo = new QComboBox(this);
    QLabel *label_format = new QLabel("Format:", this);
    QLabel *label_rate = new QLabel("Sample rate:", this);
    QLabel *label_channels = new QLabel("Channel(s):", this);
    QLabel *label_samples = new QLabel("Samples:", this);
    QLabel *label_duration = new QLabel("Duration:", this);
    QLabel *label_bitrate = new QLabel("Bitrate:", this);
    this->format = new QLabel(this);
    this->rate = new QLabel(this);
    this->channels = new QLabel(this);
    this->samples = new QLabel(this);
    this->duration = new QLabel(this);
    this->bitrate = new QLabel(this);
    /* Widgets (End) */

    /* Architecture */
    QGridLayout *main_layout = new QGridLayout;
    main_layout->addWidget(label_combo, 0, 0);
    main_layout->addWidget(this->combo, 0, 1);
    main_layout->addWidget(label_format, 2, 0);
    main_layout->addWidget(this->format, 2, 1);
    main_layout->addWidget(label_rate, 3, 0);
    main_layout->addWidget(this->rate, 3, 1);
    main_layout->addWidget(label_channels, 4, 0);
    main_layout->addWidget(this->channels, 4, 1);
    main_layout->addWidget(label_samples, 5, 0);
    main_layout->addWidget(this->samples, 5, 1);
    main_layout->addWidget(label_duration, 6, 0);
    main_layout->addWidget(this->duration, 6, 1);
    main_layout->addWidget(label_bitrate, 7, 0);
    main_layout->addWidget(this->bitrate, 7, 1);
    this->setLayout(main_layout);
    /* Architecture (End) */

    /* Connexions */
    connect(this->combo, SIGNAL (currentIndexChanged(int)), this, SLOT (updateValues(int)));
    /* Connexions (End) */
}


StreamSelectorWidget::~StreamSelectorWidget() {

}


void StreamSelectorWidget::updateStreams(const std::vector<AudioStream> & newstreams) {
    this->streams = newstreams;
    bool oldState = this->combo->blockSignals(true);
    this->combo->clear();
    for(auto const & stream: this->streams) {
        this->combo->addItem(QString("Index ") + QString::number(stream.ind_stream));
    }
    if(this->streams.size() > 0) {
        this->combo->setCurrentIndex(0);
        this->updateValues(0);
    } else {
        this->format->clear();
        this->rate->clear();
        this->channels->clear();
        this->samples->clear();
        this->duration->clear();
        this->bitrate->clear();
    }
    this->combo->blockSignals(oldState);
}


void StreamSelectorWidget::updateValues(int index) {
    // std::cout << std::endl << index << std::endl;
    // std::cout << std::endl << this->streams[index].format << std::endl;
    // std::cout << this->streams[index].format.size() << std::endl;
    // std::cout << this->streams[index].format.data() << std::endl;
    this->format->setText(QString::fromStdString(this->streams[index].format));
    this->rate->setText(QString::number(this->streams[index].sample_rate_hz / 1000.0) + QString(" KHz"));
    this->channels->setText(QString::number(this->streams[index].nb_channels));
    this->samples->setText(QString::number(this->streams[index].nb_samples));
    this->duration->setText(QString::number(this->streams[index].duration_s) + QString(" seconds"));
    this->bitrate->setText(QString::number(this->streams[index].bit_rate / 1000.0) + QString(" Kb/s"));
    emit this->indexUpdated(this->streams[index].ind_stream);
}


AudioStream StreamSelectorWidget::getCurrentStream() const {
    return this->streams[this->combo->currentIndex()];
}


void StreamSelectorWidget::clear() {
    std::vector<AudioStream> streams = {};
    this->updateStreams(streams);
}