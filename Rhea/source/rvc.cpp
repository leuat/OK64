#include "source/rvc.h"

RVC::RVC()
{

}

void RVC::Init(QByteArray *m)
{
    m_memory = m;
    m_img = QImage(256,256,QImage::Format_RGB32);
    m_img.fill(QColor(0,0,0));

}

void RVC::PutPixel(int x, int y, uchar color)
{
    m_img.setPixelColor(x,y,QColor((color&0b00000111)*32,(color&0b00011000>>3)*64,(color&0b1110000)));
}

void RVC::Update()
{
    qDebug() << "Update : " <<get(p_putpix);
    if (get(p_putpix)!=0) {
        qDebug() << "WHOO putpix!";
        PutPixel(get(p_p1_x), get(p_p1_y),get(p_p1_c));
        set(p_putpix,0);
    }

}

uchar RVC::get(uint i)
{
    return (*m_memory)[i];
}

uchar RVC::set(uint i, uchar j)
{
    (*m_memory)[i]=j;
}
