#ifndef RCOMPUTER_H
#define RCOMPUTER_H

#include <QPixmap>
#include <QThread>
#include <QImage>
#include <QDebug>
#include "source/6502/mos6502.h"
#include "source/rvc.h"
#ifdef __linux__
#include "resid/sid.h"
#endif
#ifdef __APPLE__
#include <resid/sid.h>
#endif
#ifdef _WIN32
#include "resid_win/sid.h"
#endif
#include <QAudio>
#include <QAudioBuffer>
#include <QAudioFormat>
#include <QBuffer>
#include <QAudioDeviceInfo>
#include <QAudioOutput>

class QInfiniteBuffer : public QBuffer {
public:
    qint64 getPos() {
        return m_pos;
    }
private:
    qint64 m_pos;


public:
    QInfiniteBuffer(QByteArray *buf, QObject *parent) :
        QBuffer(buf,parent)
    {
        m_pos=0;
    }

    QInfiniteBuffer(QObject *parent) :
        QBuffer(parent)
    {
        m_pos=0;
    }

    qint64 readData(char *output, qint64 maxlen) override;
};


class AThread : public QThread {

   public:
    QAudioOutput* audio;
    QBuffer* buf;
    void run() override {
        audio->start(buf);
    };

};


class RAudio : public QObject {
    Q_OBJECT
public:
    bool done = false;
    QAudioFormat audioFormat;
    QByteArray m_soundBuffer;
    QByteArray m_tempSoundBuffer;
    QAudioOutput* audio = nullptr;
    AThread m_aThread;
    int m_reset = 1;
    int m_size = 0;
    qint64 m_soundPos=0;
//    float m_bufscale = 51200;
    float m_bufscale = 15120;
//    float m_bufscale = 16;
 //   float m_bufscale = 16;
//    int m_cur = 0;
   // QInfiniteBuffer* m_input;
//    QInfiniteBuffer* m_input = nullptr;
    QBuffer* m_input = nullptr;
    void CopyBuffer();
    void Init(int samplerate, float dur);

public slots:
    void handleStateChanged(QAudio::State newState);
};


class RComputer : public QThread
{
    Q_OBJECT
public:
    RComputer();
    QPixmap m_outPut;
    bool isFirst = true;
    bool m_abort = false;
    bool m_run = false;
    int m_mhz = 985000; // mhz
    int m_fps = 50; // hz
//    int m_khz = 44100;
    int m_khz = 44100;
    int m_cyclesPerFrame = m_mhz/m_fps;
    int m_time = 0;
    int m_bpp = 4;
    int m_currentKey = 0;
    bool m_audioAction = false;
    bool m_outputBusy = false;

    OKMemory m_pram;
    RAudio m_audio;
    Mos6502 m_cpu;


    OKVC m_okvc;
#ifdef _WIN32
    SIDD m_sid;
#endif
#ifdef __linux__
    SID m_sid;
#endif
#ifdef __APPLE__
    SID m_sid;
#endif
    OKMemory m_vram;

    float m_workLoad;
    void Step();
    void Run();
    void Update();
    void PowerOn();
    void Reset();
    int LoadProgram(QString fn);

    void setKey(int k) {m_currentKey = k;}
    void HandleInput();
    void run() override;
    void Execute();
public slots:
    void onAudio();
signals:
    void emitOutput();
    void emitAudio();
};

#endif // RCOMPUTER_H
