#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&m_computer,SIGNAL(emitOutput()),this,SLOT(onEmitOutput()));
    m_computer.start();
}

MainWindow::~MainWindow()
{
    m_computer.m_abort = true;
    QThread::sleep(150);
    delete ui;
}

void MainWindow::onEmitOutput()
{
    ui->lblOutput->setPixmap(m_computer.m_outPut.scaled(320*2,200*2,Qt::KeepAspectRatio));
}
