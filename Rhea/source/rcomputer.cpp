#include "rcomputer.h"

RComputer::RComputer()
{

}

void RComputer::run()
{
    while (!m_abort) {
        QImage img(320,200,QImage::Format_RGB32);
        for (int j=0;j<200;j++)
            for (int i=0;i<320;i++)
                img.setPixelColor(i,j,QColor(rand()%255,rand()%255, rand()%255));
        m_outPut = m_outPut.fromImage(img);
        emit emitOutput();
        msleep(25);

    }
}
