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
#include <SDL_audio.h>
#endif
#ifdef __APPLE__
#include <SDL/SDL_audio.h>
#endif
#include <QBuffer>


class RAudio : public QObject {
    Q_OBJECT
public:
    bool done = false;
    static QByteArray m_soundBuffer;
    static int m_currentPos;
    static const int TYPE_DISABLED = 0;
    static const int TYPE_QT = 1;
    static const int TYPE_SDL = 2;
    int m_type = TYPE_SDL;
    int m_reset = 1;
    int m_size = 0;
    int m_time = 0;
    SDL_AudioSpec wanted;
    static qint64 m_soundPos;

    float m_bufscale = 1024;
    void Init(int samplerate, float dur);
    static void fill_audio_sdl(void *udata, Uint8 *stream, int len);
    ~RAudio();

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
