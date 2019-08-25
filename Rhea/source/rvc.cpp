#include "source/rvc.h"

RVC::RVC()
{

}

void RVC::Init(QByteArray *m)
{
    m_memory = m;
    m_img = QImage(256,256,QImage::Format_RGB32);
    m_img.fill(QColor(0,0,0));
    m_palette.resize(256);
    for (int color=0;color<256;color++)
        m_palette[color] = QColor((color&0b00000111)*32,((color&0b00011000))*8,(color&0b11100000));

}

void RVC::PutPixel(int x, int y, uchar color)
{
//    m_img.setPixelColor(x,y,QColor((color&0b00000111)*32,(color&0b00011000>>3)*64,(color&0b1110000)));
    m_img.setPixelColor(x,y,m_palette[color]);
}

void RVC::DrawLine(int x1, int y1, int x2, int y2, uchar color)
{
    QPainter painter;
    painter.begin(&m_img);
    painter.setPen(QPen(m_palette[color], 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
    painter.drawLine(x1,y1,x2,y2);
    painter.end();
}



void RVC::Update()
{
    if (get(p_putpix)!=0) {
        PutPixel(get(p_p1_x), get(p_p1_y),get(p_p1_c));
        set(p_putpix,0);
    }
    if (get(p_line)!=0) {
        DrawLine(get(p_p1_x), get(p_p1_y),get(p_p2_x), get(p_p2_y),    get(p_p1_c));
        set(p_line,0);
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
