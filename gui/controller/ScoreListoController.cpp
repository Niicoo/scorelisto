#include "ScoreListoModel.hpp"
#include "ScoreListoView.hpp"
#include "ScoreListoController.hpp"


ScoreListoController::ScoreListoController(ScoreListoModel *model, ScoreListoView *view) {
    this->model = model;
    this->view = view;
}

ScoreListoController::~ScoreListoController() {

}