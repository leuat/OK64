#ifndef RVC_H
#define RVC_H

#include <QByteArray>
#include <QImage>
#include <QDebug>
class RVC
{
public:
    const int p_p1_x = 0xF000;
    const int p_p1_y = 0xF001;
    const int p_p1_c = 0xF002;

    const int p_putpix = 0xF010;

    RVC();
    QByteArray* m_memory;
    QImage m_img;
    void Init(QByteArray* m);
    void PutPixel(int x, int y, uchar color);
    void Update();
    uchar get(uint i);
    uchar set(uint i, uchar j);
};

#endif // RVC_H
