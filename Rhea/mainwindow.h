#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include "source/rcomputer.h"
#include "source/misc/util.h"

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
    void DisplayProgram();
    void UpdateStatus();
    void keyPressEvent(QKeyEvent *e);

private:
    Ui::MainWindow *ui;

public slots:
    void onEmitOutput();
};

#endif // MAINWINDOW_H
