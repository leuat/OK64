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
    int column = 0;
    while (!done) {
        uchar c = m_computer.m_cpu.m.m_data[pc];
        QString b = Util::numToHex(c).remove("$");
        if (b.length()==1) b="0"+b;
        b="&nbsp;"+b;
        if (pc==m_computer.m_cpu.r.pc)
            b = "<b><font color=\"#2020FF\">"+b+"</font></b>";

        if (column==0) {
            b="<i><font color=\"#008000\">" + Util::numToHex(pc) + "</font>: "+b;
        }
        output +=b;
        if (cnt++>=255)
            done=true;

        column++;
        if (column==16) { output+="<br>"; column=0;}
        pc++;

    }
    Opcode cur = m_computer.m_cpu.m_opcodes[m_computer.m_cpu.m.m_data[m_computer.m_cpu.r.pc]];
    uchar b1 = m_computer.m_cpu.m.m_data[m_computer.m_cpu.r.pc+1];
    uchar b2 = m_computer.m_cpu.m.m_data[m_computer.m_cpu.r.pc+2];
    output+="<p>"+cur.m_name + "&nbsp;";
    if (cur.m_type == m_computer.m_cpu.abs)
        output += Util::numToHex(b1|b2<<8);
    if (cur.m_type == m_computer.m_cpu.imm)
        output += Util::numToHex(b1);
    ui->txtOutput->setText(output);
}

void MainWindow::UpdateStatus()
{
    QString out;
    out +="<p>&nbsp; pc = " + Util::numToHex((unsigned int)m_computer.m_cpu.r.pc);
    out +="&nbsp; a = " + Util::numToHex((uchar)m_computer.m_cpu.r.a);
    out +="&nbsp; x = " + Util::numToHex((uchar)m_computer.m_cpu.r.x);
    out +="&nbsp; y = " + Util::numToHex((uchar)m_computer.m_cpu.r.y);
    out +="&nbsp; Z = " + Util::numToHex((uchar)m_computer.m_cpu.r.Z);
    out +="&nbsp; C = " + Util::numToHex((uchar)m_computer.m_cpu.r.C);
    out +="&nbsp; Time = " + Util::numToHex((uchar)m_computer.m_cpu.m.m_data[m_computer.m_rvc.p_time]);
    out +="<br>";
    for (ushort i=0;i<32;i++)
        out+="&nbsp; " + Util::numToHex((uchar)(m_computer.m_cpu.m.m_data[m_computer.m_rvc.p_p1_x+i]));
    ui->txtStatus->setText(out);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
}

void MainWindow::onEmitOutput()
{
    ui->lblOutput->setPixmap(m_computer.m_outPut.scaled(256*2,256*2,Qt::KeepAspectRatio));
    UpdateStatus();
    DisplayProgram();
}

void MainWindow::on_btnNext_clicked()
{
    m_computer.Step();

}

void MainWindow::on_btnRun_clicked()
{
    m_computer.Run();
}
