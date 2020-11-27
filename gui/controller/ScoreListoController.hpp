#ifndef SCORE_LISTO_CONTROLLER
#define SCORE_LISTO_CONTROLLER

#include <QtCore>
#include "ScoreListoModel.hpp"
#include "ScoreListoView.hpp"

class ScoreListoController : public QObject{
    Q_OBJECT
public:
    ScoreListoController(ScoreListoModel *model, ScoreListoView *view);
    virtual ~ScoreListoController();
private:
    ScoreListoModel *model;
    ScoreListoView *view;
};

#endif /* SCORE_LISTO_CONTROLLER */
