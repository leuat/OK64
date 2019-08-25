#ifndef RVC_H
#define RVC_H

#include <QByteArray>
#include <QImage>
#include <QDebug>
#include <QPainter>
class RVC
{
public:
    const int p_p1_x = 0xFF00;
    const int p_p1_y = 0xFF01;
    const int p_p1_c = 0xFF02;
    const int p_p2_x = 0xFF04;
    const int p_p2_y = 0xFF05;
    const int p_p2_c = 0xFF06;

    const int p_putpix = 0xFF10;
    const int p_line = 0xFF11;
    QVector<QColor> m_palette;
    RVC();
    QByteArray* m_memory;
    QImage m_img;
    void Init(QByteArray* m);
    void PutPixel(int x, int y, uchar color);
    void DrawLine(int x1, int y1, int x2, int y2, uchar color);
    void Update();
    uchar get(uint i);
    uchar set(uint i, uchar j);
};

#endif // RVC_H
