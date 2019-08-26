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
    QStringList m_commands;
    int m_prevPC;
    float m_workLoad;
    void DisplayProgram();
    void UpdateStatus();
    void keyPressEvent(QKeyEvent *e);
    void SetDarkPalette();


private:
    Ui::MainWindow *ui;

public slots:
    void onEmitOutput();
private slots:
    void on_btnNext_clicked();
    void on_btnRun_clicked();
    void on_pushButton_clicked();
};

#endif // MAINWINDOW_H
