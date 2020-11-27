#include "ScoreListoModel.hpp"
#include "ScoreListoView.hpp"
#include "ScoreListoController.hpp"
#include <QApplication>
#include <QLocale>

int main(int argc, char *argv[]) {
    QLocale::setDefault(QLocale::c());
    QApplication ScoreListoApp(argc, argv);
    // ScoreListoApp.setStyleSheet("QWidget { border: 1px solid red }");
    ScoreListoModel model;
    ScoreListoView view(&model);
    // ScoreListoController controller(&model, &view);
    view.show();
    return ScoreListoApp.exec();
}