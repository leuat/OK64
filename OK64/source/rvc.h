#ifndef RVC_H
#define RVC_H

#include <QByteArray>
#include <QImage>
#include <QDebug>
#include <QPainter>

#include "source/okmemory.h"
#include "source/6502/impl.h"

class OKVC_State {
public:
    bool m_waitForVSYNC = false;
    uchar screen_pointer;

    QVector<QColor> m_palette;
    OKMemory* m_pram, *m_vram;
    mos6502* m_impl = nullptr;

};

class OKVC
{
public:
    const int p_time = 0xFFF0;
    const int p_vsync = 0xFFEF;
    const int p_fontBank = 0xFFEE;
    const int p_fontSizeX = 0xFFED;
    const int p_fontSizeY = 0xFFEC;
    const int p_borderWidth = 0xFFEB;
    const int p_borderHeight = 0xFFEA;
    const int p_borderColor = 0xFFE9;
    const int p_inputKey = 0xFFE8;
    const int p_fileLocation = 0xFF20;


    const int p_inputInterrupt = 0xFFF8;

    const int p_p1_x = 0xFF00;
    const int p_p1_y = 0xFF01;
    const int p_p1_c = 0xFF02;
    const int p_p1_3 = 0xFF03;
    const int p_p2_x = 0xFF04;
    const int p_p2_y = 0xFF05;
    const int p_p2_c = 0xFF06;
    const int p_p2_3 = 0xFF07;

    const int p_exec = 0xFF10;

    const int p_pixel = 1;
    const int p_line = 2;
    const int p_clearscreen = 3;
    const int p_circlefill = 4;
    const int p_palette = 5;
    const int p_blit = 6;
    const int p_blitFont = 7;
    const int p_rect = 8;
    const int p_resetFileList = 9;
    const int p_nextFile = 10;
    const int p_loadFile = 11;

    OKVC();
    QString m_currentDir = "/home/leuat/Dropbox/TRSE/Rhea/";

    void InsertString(QString s, int pos);
    int m_currentFile = 0;

    QImage m_img;
    QImage m_backbuffer;
    QImage m_screen;
    OKVC_State state;
    void PrepareRaster()
    {
        state.m_waitForVSYNC = false;
        state.m_pram->set(p_vsync,0);
    }
    QStringList m_listFiles;

    void ResetFileList();
    void ReadNextFile();
    void LoadFile();


    void LoadRom(QString file, int pos, bool stripHeader=false);
    void Init(OKMemory* pram, OKMemory* vram, mos6502* imp);
    void PutPixel(int x, int y, uchar color);
    void DrawCircle(int x, int y, int radius, uchar color, bool fill);
    void DrawLine(int x1, int y1, int x2, int y2, uchar color);
    void Blit(int x1, int y1, int x2, int y2, int w, int h);
    void BlitFont(int fontsizeX, int fontsizeY, int c, int col, int px, int py);
    void Rect(int x1, int y1, int w, int h,uchar c);
    void Update();
    void VRAMtoScreen();
    void GenerateOutputSignal();
    uchar get(uint i);
    uchar set(uint i, uchar j);
};

#endif // RVC_H
