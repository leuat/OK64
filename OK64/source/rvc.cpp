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
    for (int color=0;color<256;color++) {
        int k = (color&0b00011000)>>3;
        int dd=0;
//        if ((k&1)==1) dd=15;
        m_palette[color] = QColor((color&0b00000111)*32,((color&0b00011000))*8+dd,(color&0b11100000));
    }

/*    for (int color=0;color<256;color++) {
        int c = color&63;
        m_palette[color] = QColor((c&0b00000011)*64,((c&0b00001100))*16,(c&0b00110000)*4);
    }
*/
}

void RVC::PutPixel(int x, int y, uchar color)
{
//    m_img.setPixelColor(x,y,QColor((color&0b00000111)*32,(color&0b00011000>>3)*64,(color&0b1110000)));
    m_img.setPixelColor(x,y,m_palette[color]);
}

void RVC::DrawCircle(int x, int y, int radius, uchar color, bool fill)
{
    if (radius<1)
        return;
    QPainter painter;
    painter.begin(&m_img);
    painter.setPen(QPen(m_palette[color], 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
    painter.setBrush(m_palette[color]);
    painter.drawEllipse(x,y,radius,radius);
    painter.end();
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
    (*m_memory)[p_time] = rand()&255;//m_memory->at(p_time)+11;
    if (get(p_exec)==p_pixel) {
        PutPixel(get(p_p1_x), get(p_p1_y),get(p_p1_c));
        set(p_exec,0);
    }
    if (get(p_exec)==p_line) {
        DrawLine(get(p_p1_x), get(p_p1_y),get(p_p2_x), get(p_p2_y),    get(p_p1_c));
        set(p_exec,0);
    }
    if (get(p_exec)==p_clearscreen) {
        m_img.fill(m_palette[get(p_p1_c)]);
        set(p_exec,0);
    }
    if (get(p_exec)==p_circlefill) {
        DrawCircle(get(p_p1_x), get(p_p1_y),get(p_p1_3),get(p_p1_c),true);
        set(p_exec,0);
    }

    m_waitForVSYNC = (get(p_vsync)==1);
}

uchar RVC::get(uint i)
{
    return (*m_memory)[i];
}

uchar RVC::set(uint i, uchar j)
{
    (*m_memory)[i]=j;
}
