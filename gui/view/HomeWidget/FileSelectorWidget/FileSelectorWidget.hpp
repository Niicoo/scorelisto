#ifndef FILE_LOADER_WIDGET
#define FILE_LOADER_WIDGET

#include <QPushButton>
#include <QLineEdit>
#include <QString>

class FileSelectorWidget: public QWidget {
    Q_OBJECT
public:
    FileSelectorWidget(QWidget *parent=nullptr);
    ~FileSelectorWidget();
private:
    QWidget *parent;
    QPushButton *select_button;
    QLineEdit *file_line;
private slots:
    void openDialog();
public slots:
    void setDisabled(bool disable);
    void setEnabled(bool enable);
    void clear();
signals:
    void fileSelected(QString filepath);
};

#endif /* FILE_LOADER_WIDGET */
