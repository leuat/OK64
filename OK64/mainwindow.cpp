#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "source/okcomputer.h"

MainWindow::MainWindow( int type,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    if (type==0)
        m_computer = QSharedPointer<AbstractComputer>(new OkComputer());

    ui->setupUi(this);
    //SetDarkPalette();
    connect(m_computer.get(),SIGNAL(emitOutput()),this,SLOT(onEmitOutput()));
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(OnQuit()));
    connect(ui->widget,SIGNAL(emitOpenFile(QString)),this,SLOT(on_load_file(QString)));


    m_computer->PowerOn();

    this->setWindowTitle("OK64 version "+m_version);
    QCoreApplication::setApplicationVersion(m_version);
    QCoreApplication::setOrganizationDomain("lemonspawn.com");
    QCoreApplication::setOrganizationName("LemonSpawn");
    QCoreApplication::setApplicationName("OK64");
    m_settings = new QSettings();
//    std::cout << m_settings->fileName().toStdString()<<std::endl;
    DefaultUIValues();


    ui->txtStatus->setVisible(false);
    ui->txtOutput->setVisible(false);
    m_size = m_settings->value("ide/windowsize",768).toInt();
    Fit();
    //uielement->setFocusPolicy(Qt::NoFocus);
    //ui->lblOutput->setFocusPolicy(Qt::StrongFocus);
    m_computer->m_okvc.m_currentDir = ui->leDIr->text();


    QString StyleSheet = Util::loadTextFile(":resources/QTDark.stylesheet");
/*    QFile File(":resources/QTDark.stylesheet");
    File.open(QFile::ReadOnly);
    QString StyleSheet = QLatin1String(File.readAll());

    qDebug() << "EWTF " << StyleSheet;
    qApp->setStyleSheet(StyleSheet);
*/
    qApp->setStyleSheet(StyleSheet);
    UpdateShader();
//    ui->widget->showFullScreen();
//    showFullScreen();
//    ui->widget->setMaximumWidth(ui->widget->height()*1.2);
     Reset(true);

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
    ushort pc = ((m_computer->m_cpu.r.pc/16)-8)*16;

    int cnt = 0;
    int column = 0;
    while (!done) {
        uchar c = m_computer->m_pram.get(pc);
        QString b = Util::numToHex(c).remove("$");
        if (b.length()==1) b="0"+b;
        b="&nbsp;"+b;
        if (pc==m_computer->m_cpu.r.pc)
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

    pc = m_computer->m_cpu.r.pc;
    while (m_commands.count()<6)
        m_commands.append(" ");

    while (m_commands.count()>6)
        m_commands.removeFirst();
    for (int i=0;i<m_commands.count()-1;i++) {
        output+="<br>"+m_commands[i];
    }
    for (int i=0;i<8;i++) {
        QString f="";
        QString af = "";
        QString cmd = m_computer->m_cpu.getInstructionAt(pc);

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

  //  for (ushort s: m_computer.m_cpu.m_symbols.keys())
    //    output+=m_computer.m_cpu.m_symbols[s]+", ";
    ui->txtOutput->setText(output);
}

void MainWindow::UpdateStatus()
{
    QString out;
    out +="<p>&nbsp; pc = " + Util::numToHex((unsigned int)m_computer->m_cpu.r.pc);
    out +="&nbsp; a = " + Util::numToHex((uchar)m_computer->m_cpu.r.a);
    out +="&nbsp; x = " + Util::numToHex((uchar)m_computer->m_cpu.r.x);
    out +="&nbsp; y = " + Util::numToHex((uchar)m_computer->m_cpu.r.y);
    out +="&nbsp; Z = " + Util::numToHex((uchar)m_computer->m_cpu.r.Z);
    out +="&nbsp; C = " + Util::numToHex((uchar)m_computer->m_cpu.r.C);
    out +="&nbsp; N = " + Util::numToHex((uchar)m_computer->m_cpu.r.N);
//    out +="&nbsp; Time = " + Util::numToHex((uchar)m_computer.m_pram.get(m_computer.m_okvc.p_time));
    out +="<br>";
    for (ushort i=0;i<32;i++)
        out+="&nbsp; " + Util::numToHex((uchar)(m_computer->m_pram.get(m_computer->m_okvc.p_p1_x+i)));
/*    out +="<br>";
    for (ushort i=0;i<32;i++)
        out+="&nbsp; " + Util::numToHex((uchar)(m_computer.m_pram.get(0xD400+i)));
*/
    // Smoothen workload
    if (m_workLoad == 0.0)
        m_workLoad = m_computer->m_workLoad;
    m_workLoad = m_workLoad*0.95 + m_computer->m_workLoad*0.05;
    out+="<br>Workload : " + QString::number((int)m_workLoad) + "%";
    ui->txtStatus->setText(out);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{

    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        if (e->key()==Qt::Key_R) {
            Reset();
            return;
        }
    }

    QVector<int> arrowKeys({18,19,20,21});
    if (e->text().count()==0 && !arrowKeys.contains((char)e->key()))
        return;
    char c = e->key();
    if (e->text().count()>0)
        c = e->text().at(0).toLatin1();
//    qDebug() << c;
    m_computer->setKey(c);
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
    setFixedSize(m_size*(visible+1),m_size*1.2);
    m_settings->setValue("ide/windowsize", m_size);
}

void MainWindow::DefaultUIValues()
{
    //qDebug() << "POES " <<m_settings->value("ui/sldcrt",40).toInt();
    ui->sldCrt->setValue(m_settings->value("ui/sldcrt",14).toInt());
    ui->sldSat->setValue(m_settings->value("ui/sldsat",50).toInt());
    ui->sldGamma->setValue(m_settings->value("ui/sldgam",40).toInt());
    ui->sldScan->setValue(m_settings->value("ui/sldscan",10).toInt());
    ui->sldChrom->setValue(m_settings->value("ui/sldchrom",20).toInt());

    ui->tabMain->setCurrentIndex(m_settings->value("ui/curtab",1).toInt());
    ui->leDIr->setText(m_settings->value("ui/curdir","").toString());

/*    m_settings->setValue("ui/sldsat",ui->sldSat->value());
    m_settings->setValue("ui/sldgam",ui->sldGamma->value());
    m_settings->setValue("ui/sldscan",ui->sldScan->value());
    m_settings->setValue("ui/sldchrom",ui->sldChrom->value());
    m_settings->setValue("ui/sldcrt",ui->sldCrt->value());
  */

}

void MainWindow::Reset(bool first)
{
    // Hard cap in reset
    if (ui->widget->time<50 && ui->widget->time!=0.0)
        return;

    on_load_file(":resources/rom/kos.prg");
}

void MainWindow::ResetFocus()
{
    //ui->lblOutput->setFocus();
    ui->widget->setFocus();
}

void MainWindow::onEmitOutput()
{
//    qDebug() << m_count;
    if (m_computer->m_abort)
        return;



    m_computer->m_outputBusy=true;
//    m_computer.m_okvc.GenerateOutputSignal();
    m_computer->m_outPut = m_computer->m_outPut.fromImage(m_computer->m_okvc.m_screen, Qt::AutoColor);

//    ui->lblOutput->setPixmap(m_computer.m_outPut.scaled(256*2,256*2,Qt::KeepAspectRatio));

    ui->widget->setTexture(m_computer->m_okvc.m_screen);
//    ui->widget->setFixedSize(m_size, m_size*0.9);

    if (ui->txtStatus->isVisible()) {
        if ((m_count&1)==0)
            UpdateStatus();
        else
            DisplayProgram();
    }
    m_count++;
    m_computer->m_outputBusy=false;

}

void MainWindow::on_load_file(QString filename)
{

    m_computer->m_run =false;
    m_computer->msleep(25);
    QThread::msleep(25);


    m_computer->LoadProgram(filename);
    m_computer->Reset();
    ui->widget->Reset();
    m_computer->m_run =true;

}

void MainWindow::on_btnNext_clicked()
{
    m_computer->Step();
    ResetFocus();
}

void MainWindow::on_btnRun_clicked()
{
    m_computer->Run();
    ResetFocus();
}

void MainWindow::on_pushButton_clicked()
{
    m_computer->m_run = !m_computer->m_run;
    if (m_computer->m_run)
        ui->pushButton->setText("Pause");
    else
        ui->pushButton->setText("Continue");

    ResetFocus();
}

void MainWindow::on_pushButton_2_clicked()
{

    ui->txtStatus->setVisible(!ui->txtStatus->isVisible());
    ui->txtOutput->setVisible(!ui->txtOutput->isVisible());
    Fit();
    ResetFocus();
}

void MainWindow::onQuit()
{
//    m_settings.
    m_settings->setValue("ui/curtab",ui->tabMain->currentIndex());

    m_computer->m_abort = true;
    m_computer->msleep(250);
    QThread::msleep(250);
}

void MainWindow::on_leDIr_textChanged(const QString &arg1)
{
    m_computer->m_okvc.m_currentDir = arg1;
    m_settings->setValue("ui/curdir",arg1);

}

void MainWindow::on_pushButton_3_clicked()
{
    Reset();
    ResetFocus();

}

void MainWindow::on_sldCrt_valueChanged(int value)
{
    m_settings->setValue("ui/sldcrt",ui->sldCrt->value());
    UpdateShader();
}

void MainWindow::UpdateShader()
{
    float crt = ui->sldCrt->value()/100.0; // 0-1
    float bs = 1.2*(1-crt*0.2);
    ui->widget->CD = QVector3D(0.3,0.6,0)*crt;
    ui->widget->barrelScale =  (QVector3D(bs,bs,bs) - (0.40 * ui->widget->CD)*crt);
    ui->widget->saturation = ui->sldSat->value()/100.0*3; // 0-3
    ui->widget->gamma = ui->sldGamma->value()/100.0*2; // 0-3
    ui->widget->lamp = ui->sldScan->value()/100.0*2; // 0-3
    ui->widget->chromatic = ui->sldChrom->value()/3000.0;

//    qDebug() << m_settings->value("ui/sldcrt",40).toInt();
}

void MainWindow::on_sldSat_valueChanged(int value)
{
    m_settings->setValue("ui/sldsat",ui->sldSat->value());
    UpdateShader();
}

void MainWindow::on_sldGamma_valueChanged(int value)
{
    m_settings->setValue("ui/sldgam",ui->sldGamma->value());
    UpdateShader();
}

void MainWindow::on_sldScan_valueChanged(int value)
{
    m_settings->setValue("ui/sldscan",ui->sldScan->value());
    UpdateShader();
}

void MainWindow::on_sldChrom_valueChanged(int value)
{
    m_settings->setValue("ui/sldchrom",ui->sldChrom->value());
    UpdateShader();
}

void MainWindow::on_actionLoad_prorgram_from_file_triggered()
{
   QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open .prg file"), "", tr("OK64 programs (*.prg)"));
   if (fileName=="")
       return;
   m_computer->LoadProgram(fileName);

}

void MainWindow::on_action_Reset_system_triggered()
{
    Reset();
}

void MainWindow::on_action_Quit_triggered()
{
    QApplication::exit(0);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    //m_size = this->size().width()-10;
    Fit();
}

void MainWindow::on_action1280_triggered()
{
    m_size = 1280;
    Fit();
}
void MainWindow::on_action1024_triggered()
{
    m_size = 1024;
    Fit();
}
void MainWindow::on_action768_triggered()
{
    m_size = 768;
    Fit();
}
void MainWindow::on_action512_triggered()
{
    m_size = 512;
    Fit();
}

void MainWindow::on_btnSetDir_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose directory with prg files"), ".", QFileDialog::ReadOnly);
    if (path!="")
        ui->leDIr->setText(path);
}

void MainWindow::on_action640x640_triggered()
{
    m_size = 640;
    Fit();

}

void MainWindow::on_actionAbout_triggered()
{
    DialogAbout* da = new DialogAbout();
    da->exec();
    delete da;
}

void MainWindow::on_action256x256_triggered()
{
    m_size = 256;
    Fit();

}
