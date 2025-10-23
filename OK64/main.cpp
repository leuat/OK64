#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int c=1;
    int system = 0;

    if (argc!=1) {
        //        qDebug() << "Argc " << argc;
        if (QString(argv[c]).startsWith("-")) {
            if (QString(argv[c])=="-c64")
                system = 1;
        }
    }
    MainWindow w(system);
    if (argc!=1) {
        //        qDebug() << "Argc " << argc;
        while (QString(argv[c]).startsWith("-")) {
            bool ok=false;
            if (QString(argv[c])=="-moncommands") {
                QString symFile = argv[c+1];
                w.m_computer->m_cpu.LoadSybols(symFile);
                c+=2;
                ok=true;
            }

            if (!ok) {
                qDebug() << "Unknown flag : " << argv[c];
                exit(1);
            }
        }
        QString file = argv[c];
        w.m_computer->LoadProgram(file);
    }
    else {

        //        w.Reset();

    }

    w.m_computer->start();

    a.setOrganizationDomain("lemonspawn.com");
    a.setApplicationName("OK64");

    w.show();
    return a.exec();
}
