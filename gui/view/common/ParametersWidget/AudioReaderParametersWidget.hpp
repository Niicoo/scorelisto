#ifndef AUDIO_READER_PARAMETERS_WIDGET
#define AUDIO_READER_PARAMETERS_WIDGET

#include "AudioReader.hpp"
#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>


class AudioReaderParametersWidget: public QGroupBox {
    Q_OBJECT
public:
    AudioReaderParametersWidget(QWidget *parent=nullptr);
    ~AudioReaderParametersWidget();
    bool isReady();
    AudioReaderParameters getParameters();
private:
    QCheckBox *resampled;
    QLineEdit *windows_size;
    QLineEdit *period;
    QLineEdit *resample_rate;
    bool ready;
private slots:
    void checkParameters();
    void enableResampling(int state);
public slots:
    void setParameters(const AudioReaderParameters & parameters);
    void setDefaultParameters();
signals:
    void parametersReady();
    void parametersNotReady();
};

#endif /* AUDIO_READER_PARAMETERS_WIDGET */
