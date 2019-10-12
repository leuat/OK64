#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    int c=1;
    if (argc!=1) {
//        qDebug() << "Argc " << argc;
        while (QString(argv[c]).startsWith("-")) {
            bool ok=false;
            if (QString(argv[c])=="-moncommands") {
             QString symFile = argv[c+1];
             w.m_computer.m_cpu.LoadSybols(symFile);
             c+=2;
             ok=true;
            }

            if (!ok) {
                qDebug() << "Unknown flag : " << argv[c];
                exit(1);
            }
        }
        QString file = argv[c];
        w.m_computer.LoadProgram(file);
        w.m_computer.m_run =true;
    }
    else {
//        w.m_computer.LoadProgram("/home/leuat/Dropbox/TRSE/Rhea/okcomputer.prg");
  //      w.m_computer.m_cpu.LoadSybols("/home/leuat/Dropbox/TRSE/Rhea/okcomputer.sym");

        w.m_computer.LoadProgram("/home/leuat/Dropbox/TRSE/Rhea/kos.prg");
        w.m_computer.m_cpu.LoadSybols("/home/leuat/Dropbox/TRSE/Rhea/kos.sym");
        w.m_computer.m_run =true;

//        w.m_computer.m_cpu.m_breakPoints.append(0x133b);
//        w.m_computer.m_cpu.m_breakPoints.append(0x1179);
    }
    w.show();
    return a.exec();
}
