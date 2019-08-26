#ifndef RCOMPUTER_H
#define RCOMPUTER_H

#include <QPixmap>
#include <QThread>
#include <QImage>
#include <QDebug>
#include "source/6502/mos6502.h"
#include "source/rvc.h"
#include "resid/sid.h"
#include <QAudio>
#include <QAudioBuffer>
#include <QAudioFormat>
#include <QBuffer>
#include <QAudioDeviceInfo>
#include <QAudioOutput>

class RAudio : public QObject {
    Q_OBJECT
public:
    QAudioFormat audioFormat;
    QByteArray m_soundBuffer;
    bool isPlaying = false;
    int m_cur,m_size = 0;

    void Init();

    void Audio();

};


class RComputer : public QThread
{
    Q_OBJECT
public:
    RComputer();
    QPixmap m_outPut;
    bool m_abort = false;
    bool m_run = false;
    int m_mhz = 2000000; // mhz
    int m_fps = 50; // hz
    int m_cyclesPerFrame = m_mhz/m_fps;
    int m_time = 0;
    RAudio m_audio;
    Mos6502 m_cpu;

    RVC m_rvc;
    SID m_sid;
    float m_workLoad;
    void Step();
    void Run();
    void Update();
    void PowerOn();
    void run();
public slots:
    void onAudio();
signals:
    void emitOutput();
    void emitAudio();
};

#endif // RCOMPUTER_H
