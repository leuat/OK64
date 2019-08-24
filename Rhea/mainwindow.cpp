#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&m_computer,SIGNAL(emitOutput()),this,SLOT(onEmitOutput()));
    m_computer.PowerOn();


    m_computer.start();
}

MainWindow::~MainWindow()
{
    m_computer.m_abort = true;
    QThread::sleep(150);
    delete ui;
}

void MainWindow::DisplayProgram()
{
    bool done = false;
    QString output = "<p>";
    int pc = Constants::c.programStartAddress;

    int cnt = 0;
    while (!done) {
        uchar c = m_computer.m_cpu.m.m_data[pc];
        QString b = Util::numToHex(c).remove("$");
        if (b.length()==1) b="0"+b;

        if (pc==m_computer.m_cpu.r.pc)
            b = "<b><font color=\"#4040F0\">"+b+"</font></b>";

        output +="&nbsp;"+b;
        if (cnt++>=251)
            done=true;

        pc++;

    }
    ui->txtOutput->setText(output);
}

void MainWindow::UpdateStatus()
{
    QString out;
    out +="<p>&nbsp; pc = " + Util::numToHex((unsigned int)m_computer.m_cpu.r.pc);
    out +="&nbsp; a = " + Util::numToHex((uchar)m_computer.m_cpu.r.a);
    out +="&nbsp; x = " + Util::numToHex((uchar)m_computer.m_cpu.r.x);
    out +="&nbsp; y = " + Util::numToHex((uchar)m_computer.m_cpu.r.y);
    ui->txtStatus->setText(out);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key()==Qt::Key_Space)

        m_computer.m_cpu.Eat();
}

void MainWindow::onEmitOutput()
{
    ui->lblOutput->setPixmap(m_computer.m_outPut.scaled(320*2,200*2,Qt::KeepAspectRatio));
    UpdateStatus();
    DisplayProgram();
}
