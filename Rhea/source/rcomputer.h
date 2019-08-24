#ifndef RCOMPUTER_H
#define RCOMPUTER_H

#include <QPixmap>
#include <QThread>
#include <QImage>
#include <QDebug>

class RComputer : public QThread
{
    Q_OBJECT
public:
    RComputer();
    QPixmap m_outPut;

    bool m_abort = false;

    void Update();
    void run();
signals:
    void emitOutput();
};

#endif // RCOMPUTER_H
