#ifndef HOME_WIDGET
#define HOME_WIDGET

#include <StreamSelectorWidget.hpp>
#include <AudioPlayerWidget.hpp>
#include <RecorderWidget.hpp>
#include <FileSelectorWidget.hpp>
#include "ScoreListoModel.hpp"
#include "ProgressBarWidget.hpp"
#include <QWidget>
#include <QString>
#include <QCheckBox>

class HomeWidget: public QWidget {
    Q_OBJECT
public:
    HomeWidget(QWidget *parent=nullptr);
    ~HomeWidget();
    ProgressBarWidget *progress_bar;
private:
    FileSelectorWidget *fileselector;
    RecorderWidget *recorder;
    StreamSelectorWidget *streamselector;
    AudioPlayerWidget *player;
    QPushButton *convert_button;
    QCheckBox *advanced_opt;
public slots:
    void fileLoaded(std::vector<AudioStream> streams);
    void fileFailed();
private slots:
    void clear();
    void emitConversion();
    void enableConvertButton();
    void disableConvertButton();
    void fileOpened(QString filename);
    void updatePlayer();
signals:
    void runFileOpening(std::string);
    void directConversionClicked(AudioStream);
    void advancedConversionClicked(AudioStream);
};

#endif /* HOME_WIDGET */
