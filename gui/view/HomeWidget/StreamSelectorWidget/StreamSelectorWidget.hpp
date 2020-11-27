#ifndef STREAM_SELECTOR_WIDGET
#define STREAM_SELECTOR_WIDGET

#include <QWidget>
#include <AudioReader.hpp>
#include <QComboBox>
#include <QLabel>

class StreamSelectorWidget: public QWidget {
    Q_OBJECT
public:
    StreamSelectorWidget(QWidget *parent=nullptr);
    ~StreamSelectorWidget();
    AudioStream getCurrentStream() const;
public slots:
    void updateStreams(const std::vector<AudioStream> & newstreams);
    void clear();
private slots:
    void updateValues(int index);
signals:
    void indexUpdated(int);
private:
    std::vector<AudioStream> streams;
    QComboBox *combo;
    QLabel *format;
    QLabel *rate;
    QLabel *channels;
    QLabel *samples;
    QLabel *duration;
    QLabel *bitrate;
};

#endif /* STREAM_SELECTOR_WIDGET */
