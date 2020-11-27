#ifndef RECORDER_WIDGET
#define RECORDER_WIDGET

#include <QWidget>

class RecorderWidget: public QWidget {
    Q_OBJECT
public:
    RecorderWidget(QWidget *parent=nullptr);
    ~RecorderWidget();
signals:
    void fileRecorded(QString filepath);
public slots:
    void clear();
};

#endif /* RECORDER_WIDGET */
