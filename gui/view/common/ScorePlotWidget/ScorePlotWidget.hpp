#ifndef SCORE_PLOT_WIDGET
#define SCORE_PLOT_WIDGET

#include <QWidget>


class ScorePlotWidget: public QWidget {
    Q_OBJECT
public:
    ScorePlotWidget(QWidget *parent=nullptr);
    ~ScorePlotWidget();
public slots:
    void clear();
};

#endif /* SCORE_PLOT_WIDGET */
