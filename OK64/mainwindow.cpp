#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    SetDarkPalette();
    connect(&m_computer,SIGNAL(emitOutput()),this,SLOT(onEmitOutput()));
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(OnQuit()));

    m_computer.PowerOn();


    m_computer.start();

    ui->txtStatus->setVisible(false);
    ui->txtOutput->setVisible(false);

    Fit();
    //uielement->setFocusPolicy(Qt::NoFocus);
    ui->lblOutput->setFocusPolicy(Qt::StrongFocus);
}

MainWindow::~MainWindow()
{
    onQuit();
    delete ui;
}

void MainWindow::DisplayProgram()
{
    bool done = false;
    QString output = "<p>";
    ushort pc = ((m_computer.m_cpu.r.pc/16)-8)*16;

    int cnt = 0;
    int column = 0;
    while (!done) {
        uchar c = m_computer.m_pram.get(pc);
        QString b = Util::numToHex(c).remove("$");
        if (b.length()==1) b="0"+b;
        b="&nbsp;"+b;
        if (pc==m_computer.m_cpu.r.pc)
            b = "<b><font color=\"#8080FF\">"+b+"</font></b>";

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

    pc = m_computer.m_cpu.r.pc;
    while (m_commands.count()<4)
        m_commands.append(" ");

    while (m_commands.count()>4)
        m_commands.removeFirst();
    for (int i=0;i<m_commands.count()-1;i++) {
        output+="<br>"+m_commands[i];
    }
    for (int i=0;i<4;i++) {
        QString f="";
        QString af = "";
        QString cmd = m_computer.m_cpu.getInstructionAt(pc);

            if (i==0) {
                f="<font color=\"#80FF80\">";
                af="</font>";
                if (m_prevPC!=pc) {
                    m_prevPC = pc;
                    m_commands.append(cmd);
                }

            }
//            if (i!=0)
            output+="<br>"+f+cmd+af;
        }

    for (ushort s: m_computer.m_cpu.m_symbols.keys())
        output+=m_computer.m_cpu.m_symbols[s]+", ";
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
    out +="&nbsp; N = " + Util::numToHex((uchar)m_computer.m_cpu.r.N);
    out +="&nbsp; Time = " + Util::numToHex((uchar)m_computer.m_pram.get(m_computer.m_okvc.p_time));
    out +="<br>";
    for (ushort i=0;i<32;i++)
        out+="&nbsp; " + Util::numToHex((uchar)(m_computer.m_pram.get(m_computer.m_okvc.p_p1_x+i)));
    out +="<br>";
    for (ushort i=0;i<32;i++)
        out+="&nbsp; " + Util::numToHex((uchar)(m_computer.m_pram.get(0xD400+i)));

    // Smoothen workload
    if (m_workLoad == 0.0)
        m_workLoad = m_computer.m_workLoad;
    m_workLoad = m_workLoad*0.98 + m_computer.m_workLoad*0.02;
    out+="<br>Workload : " + QString::number((int)m_workLoad) + "%";
    ui->txtStatus->setText(out);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->text().count()==0)
        return;
    char c = e->key();
    if (e->text().count()>0)
        c = e->text().at(0).toLatin1();
    m_computer.setKey(c);
}

void MainWindow::SetDarkPalette() {
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    QApplication::setPalette(darkPalette);
    qApp->setStyleSheet("QToolTip { color: #ffE0C0; background-color: #000000; border: 0px; }");


}

void MainWindow::Fit()
{
    bool visible = ui->txtOutput->isVisible();
    ui->hlMain->setStretch(0,1);
    ui->hlMain->setStretch(1,1*visible);
    setFixedSize(800*(visible+1),800);

}

void MainWindow::onEmitOutput()
{
//    qDebug() << m_count;
    if (m_computer.m_abort)
        return;
    m_computer.m_outputBusy=true;
//    m_computer.m_okvc.GenerateOutputSignal();
    m_computer.m_outPut = m_computer.m_outPut.fromImage(m_computer.m_okvc.m_screen, Qt::AutoColor);

    ui->lblOutput->setPixmap(m_computer.m_outPut.scaled(256*2,256*2,Qt::KeepAspectRatio));
    if (ui->txtStatus->isVisible() && ((m_count&3)==0)) {
        UpdateStatus();
        DisplayProgram();
    }
    m_count++;
    m_computer.m_outputBusy=false;

}

void MainWindow::on_btnNext_clicked()
{
    m_computer.Step();

}

void MainWindow::on_btnRun_clicked()
{
    m_computer.Run();
}

void MainWindow::on_pushButton_clicked()
{
    m_computer.m_run = false;
}

void MainWindow::on_pushButton_2_clicked()
{

    ui->txtStatus->setVisible(!ui->txtStatus->isVisible());
    ui->txtOutput->setVisible(!ui->txtOutput->isVisible());
    Fit();
}

void MainWindow::onQuit()
{
    m_computer.m_abort = true;
    m_computer.msleep(250);
    QThread::msleep(250);
}
