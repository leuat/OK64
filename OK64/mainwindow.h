#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include "source/okcomputer.h"
#include "source/misc/util.h"
#include <QFileDialog>
#include <QSettings>
#include "source/dialogabout.h"
#include <QSharedPointer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int type, QWidget *parent = nullptr);
    ~MainWindow();
    QSharedPointer<AbstractComputer> m_computer;
    QStringList m_commands;
    int m_count=0;
    int m_size = 900;
    int m_prevPC;
    QString m_version = "1.02";
    float m_workLoad;
    void DisplayProgram();
    void UpdateStatus();
    void keyPressEvent(QKeyEvent *e) override;
    void SetDarkPalette();
    void Fit();
    void DefaultUIValues();
    QSettings* m_settings = nullptr;

    void Reset(bool first=false);
    void ResetFocus();
private:
    Ui::MainWindow *ui;

public slots:
    void onEmitOutput();
private slots:
    void on_load_file(QString filename);
    void on_btnNext_clicked();
    void on_btnRun_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void onQuit();
    void on_leDIr_textChanged(const QString &arg1);
    void on_pushButton_3_clicked();
    void on_sldCrt_valueChanged(int value);
    void UpdateShader();
    void on_sldSat_valueChanged(int value);
    void on_sldGamma_valueChanged(int value);
    void on_sldScan_valueChanged(int value);
    void on_sldChrom_valueChanged(int value);
    void on_actionLoad_prorgram_from_file_triggered();
    void on_action_Reset_system_triggered();
    void on_action_Quit_triggered();
    void resizeEvent (QResizeEvent *event) override;
    void on_action1280_triggered();
    void on_action1024_triggered();
    void on_action768_triggered();
    void on_action512_triggered();
    void on_btnSetDir_clicked();
    void on_action640x640_triggered();
    void on_actionAbout_triggered();
    void on_action256x256_triggered();
};

#endif // MAINWINDOW_H
