#ifndef RCOMPUTER_H
#define RCOMPUTER_H

#include <QPixmap>
#include <QThread>
#include <QImage>
#include <QDebug>
#include "source/6502/mos6502.h"

class RComputer : public QThread
{
    Q_OBJECT
public:
    RComputer();
    QPixmap m_outPut;

    bool m_abort = false;

    Mos6502 m_cpu;

    void Update();
    void PowerOn();
    void run();
signals:
    void emitOutput();
};

#endif // RCOMPUTER_H
