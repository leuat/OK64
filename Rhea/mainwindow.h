#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "source/rcomputer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    RComputer m_computer;
private:
    Ui::MainWindow *ui;

public slots:
    void onEmitOutput();
};

#endif // MAINWINDOW_H
