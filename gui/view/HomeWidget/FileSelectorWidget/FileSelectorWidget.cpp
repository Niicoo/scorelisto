#include "FileSelectorWidget.hpp"
#include <QWidget>
#include <QString>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDir>
#include <iostream>

FileSelectorWidget::FileSelectorWidget(QWidget *parent/*=nullptr*/): QWidget(parent) {
    /* Widgets */
    this->select_button = new QPushButton("Select a file", this);
    this->file_line = new QLineEdit(this);
    this->file_line->setReadOnly(true);
    /* Widgets (End)*/

    /* Design */
    this->select_button->setFixedSize(300, 100);
    this->file_line->setFixedSize(300, 20);
    /* Design (End) */

    /* Architecture */
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addWidget(this->select_button, 0, Qt::AlignCenter);
    main_layout->addWidget(this->file_line, 0, Qt::AlignCenter);
    this->setLayout(main_layout);
    /* Architecture (End) */

    /* Connexions */
    connect(this->select_button, SIGNAL (clicked()),this, SLOT (openDialog()));
    /* Connexions (End) */
}


FileSelectorWidget::~FileSelectorWidget() {

}


void FileSelectorWidget::clear() {
    this->file_line->clear();
}


void FileSelectorWidget::openDialog() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath(), tr("Audio / Video  (*.*)"));
    if(!fileName.isEmpty()) {
        this->file_line->setText(fileName);
        emit this->fileSelected(fileName);
    }
}


void FileSelectorWidget::setDisabled(bool disable) {
    this->select_button->setDisabled(disable);
    this->file_line->setDisabled(disable);
}


void FileSelectorWidget::setEnabled(bool enable) {
    this->select_button->setEnabled(enable);
    this->file_line->setEnabled(enable);
}
