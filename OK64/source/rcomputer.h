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

class QInfiniteBuffer : public QBuffer {
public:
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

    qint64 readData(char *output, qint64 maxlen)
    {
        qint64 outputpos=0;
        const QByteArray &d=data();

        do
        {
            qint64 sizetocopy=maxlen-outputpos;
            if((maxlen-outputpos)>(d.size()-m_pos))
                sizetocopy=d.size()-m_pos;
            memcpy(output,d.constData()+m_pos,sizetocopy);
            outputpos+=sizetocopy;
            m_pos+=sizetocopy;
            if(m_pos>=d.size())
                m_pos=0;
        } while(outputpos<maxlen);

        return maxlen;
    }
};


class RAudio : public QObject {
    Q_OBJECT
public:
    bool done = false;
    QAudioFormat audioFormat;
    QByteArray m_soundBuffer;
    QByteArray m_tempSoundBuffer;
    QAudioOutput* audio;
    int m_size = 0;
    QInfiniteBuffer* m_input;

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
    bool m_abort = false;
    bool m_run = false;
    int m_mhz = 985000; // mhz
    int m_fps = 50; // hz
//    int m_khz = 44100;
    int m_khz = 44100;
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
