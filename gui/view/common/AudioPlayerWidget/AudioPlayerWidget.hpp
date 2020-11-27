#ifndef AUDIO_PLAYER_WIDGET
#define AUDIO_PLAYER_WIDGET

#include <QWidget>

class AudioPlayerWidget: public QWidget {
    Q_OBJECT
public:
    AudioPlayerWidget(QWidget *parent = nullptr);
    ~AudioPlayerWidget();
private:
    std::string filename;
    int ind_stream;
public slots:
    void clear();
    void updateAudio(std::string filename, int ind_stream);
    void play();
    void stop();
    void pause();
};

#endif /* AUDIO_PLAYER_WIDGET */
