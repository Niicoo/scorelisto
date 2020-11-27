#ifndef PROGRESS_BAR_WIDGET
#define PROGRESS_BAR_WIDGET
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

class ProgressBarWidget: public QWidget {
    Q_OBJECT
public:
    ProgressBarWidget(QWidget *parent=nullptr);
    ~ProgressBarWidget();
private:
    QProgressBar *bar;
    QPushButton *cancel;
    QPushButton *dl_pitch_button;
    QPushButton *dl_step_button;
    QPushButton *dl_score_button;
    QLabel *pitchlabel;
    QLabel *steplabel;
    QLabel *notelabel;
    QLabel *pitchcursor;
    QLabel *stepcursor;
    QLabel *notecursor;
public slots:
    void clear();
    void updateProgression(float progress);

    void setPitchConversionStarted();
    void setStepConversionStarted();
    void setNoteConversionStarted();

    void setPitchConversionFinished();
    void setStepConversionFinished();
    void setNoteConversionFinished();

    void setPitchConversionFailed();
    void setStepConversionFailed();
    void setNoteConversionFailed();
signals:
    void cancelClicked();
    void downloadPitchClicked();
    void downloadStepClicked();
    void downloadScoreClicked();
};

#endif /* PROGRESS_BAR_WIDGET */
