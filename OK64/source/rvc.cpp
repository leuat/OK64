#include "source/rvc.h"
#include <QFile>
#include <QDirIterator>
#include <QVector3D>
OKVC::OKVC()
{
}

void OKVC::InsertString(QString s, int pos)
{
    for (int i=0;i<s.length();i++)
        state.m_pram->set(pos++,s[i].toLatin1());
    state.m_pram->set(pos,0);
}

void OKVC::StripPramToVram()
{
    int sp = 256*state.m_pram->get(p_curStrip);
    sp+=16*state.m_pram->get(p_dstPage)*256;
    for (int i=0;i<256;i++)
        state.m_vram->set(i+sp,
                          state.m_pram->get(i+p_strip));
}

void OKVC::StripVramToPram()
{
    int sp = +256*state.m_pram->get(p_curStrip);
    sp+=16*state.m_pram->get(p_dstPage)*256;
    for (int i=0;i<256;i++)
        state.m_pram->set(i+p_strip,
            state.m_vram->get(i+sp));


}

void OKVC::ResetFileList()
{
    m_listFiles.clear();
    m_currentFile = 0;
    InsertString("               ",p_fileLocation);
    QDir d(m_currentDir);
    if (!d.exists()) {
        InsertString("Invalid file directory",p_fileLocation);
        return;
    }
    QDirIterator it(m_currentDir, QStringList() << "*.prg", QDir::Files);
    while (it.hasNext()) {
        QString s = it.next();
        m_listFiles.append(s.split("/").last());
//        qDebug() << it.next();
    }
    InsertString(m_listFiles[m_currentFile],p_fileLocation);
}

void OKVC::ReadNextFile()
{
    m_currentFile++;
    if (m_currentFile<m_listFiles.count())
        InsertString(m_listFiles[m_currentFile],p_fileLocation);
    else InsertString("",p_fileLocation);
}

void OKVC::LoadFile()
{
    QString f = "";
    int pos = p_fileLocation;
    while (state.m_pram->get(pos)!=0){
        f+=QChar(state.m_pram->get(pos));
        pos++;
    }
//    qDebug() << m_currentDir + f;
    if (!QFile::exists(m_currentDir + f))
        return;

    LoadRom(m_currentDir + f,0,true);
//    state.m_impl->Run(0x400);
    state.m_impl->pc = 0x400;
    Defaults();
}

bool OKVC::InputVectorSet()
{
    return (!(state.m_pram->get(p_inputInterrupt)==0 && state.m_pram->get(p_inputInterrupt+1)==0));
}

void OKVC::Defaults()
{
    state.m_pram->set(p_fontBank,0x0F);
    state.m_pram->set(p_fontSizeX,0x08);
    state.m_pram->set(p_fontSizeY,0x08);
    state.m_pram->set(p_borderColor,0x0);
    state.m_pram->set(p_borderWidth,0x10);
    state.m_pram->set(p_borderHeight,0x10);
    state.m_pram->set(p_srcPage,16);
    state.m_pram->set(p_dstPage,0);
    state.m_pram->set(p_inputInterrupt,0);
    state.m_pram->set(p_inputInterrupt+1,0);

}

void OKVC::LoadRom(QString fn, int startpos, bool useHeader)
{
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly)) return;
    QByteArray blob = file.readAll();
    int pos = startpos;
    if (useHeader) {
        pos = blob[1]*0x100 + blob[0];
        blob.remove(0,2);
    }
    int programSize = min(blob.size()+pos,65536);

    if (startpos<65536) {
        for (int i=0;i<programSize;i++)
            if (i+pos<0xFE00)
                state.m_pram->set(i+pos,blob[i]);

        blob.remove(0,programSize);
    }
//    return;
    if (pos<65536)
        pos = 0;
    if (blob.size()>0) {
        for (int i=0;i<blob.size();i++)
            state.m_vram->set(i+pos,blob[i]);
//        qDebug() << "HERE " << fn << QString::number(pos,16);

    }
//    LoadRom(":resources/rom/font.bin",0xF0000,false);

//    m_okvc.LoadRom(fn,0x400,true);
  //  m_okvc.VRAMtoScreen();


}

void OKVC::Init(OKMemory* pram, OKMemory* vram, mos6502* imp)
{
    state.m_impl = imp;
    state.m_pram = pram;
    state.m_vram = vram;
    srand(time(nullptr));
//    qDebug() << "RAND" << (rand()%256);
//    m_rand = QRandomGenerator::system();

    m_img = QImage(256,256,QImage::Format_RGB32);
    m_backbuffer = QImage(256,256,QImage::Format_RGB32);
    m_screen = QImage(256,256,QImage::Format_RGB32);

    state.m_palette.resize(256);
    for (int color=0;color<256;color++) {
        int k = (color&0b00011000)>>3;
        int dd=0;
//        if ((k&1)==1) dd=15;
        QVector3D col = QVector3D((color&0b00000111)*32,((color&0b00011000))*8+dd,(color&0b11100000));
        float c = (col.x()+col.y()+col.z())/3.0;
        QVector3D s(0.8,0.2,0.6);
//        s = QVector3D(1,1,0.5);
        col.setX(c*(1-s.x())+col.x()*(s.x()));
        col.setY(c*(1-s.y())+col.y()*(s.y()));
        col.setZ(c*(1-s.z())+col.z()*(s.z()));
        state.m_palette[color] = Util::toColor(col);
    }
//    qDebug() << QString::number(state.m_pram->get(p_fontBank));
    LoadRom(":resources/rom/font.bin",0xF0000,false);
    Defaults();


}

void OKVC::PutPixel(int x, int y, uchar color)
{
//    m_img.setPixelColor(x,y,QColor((color&0b00000111)*32,(color&0b00011000>>3)*64,(color&0b1110000)));
    m_img.setPixelColor(x,y,QColor(color,0,0));
}

void OKVC::DrawCircle(int x, int y, int radius, uchar color, bool fill)
{
    if (radius<1)
        return;
    QPainter painter;
    painter.begin(&m_img);
    painter.setPen(QPen(QColor(color,0,0), 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
    painter.setBrush(QColor(color,0,0));
    painter.drawEllipse(x,y,radius,radius);
    painter.end();
}


void OKVC::DrawPolygon(int x1, int y1, int x2,int y2, int x3, int y3, uchar color, bool fill)
{
    QPainter painter;

    painter.begin(&m_img);
    painter.setPen(QPen(QColor(color,0,0), 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
    painter.setBrush(QColor(color,0,0));
 //   painter.drawEllipse(x,y,radius,radius);
    QVector<QPoint> points;
    points.append(QPoint(x1,y1));
    points.append(QPoint(x2,y2));
    points.append(QPoint(x3,y3));
    painter.drawPolygon(points);
    painter.end();
}

void OKVC::DrawLine(int x1, int y1, int x2, int y2, uchar color)
{
    QPainter painter;
    painter.begin(&m_img);
    painter.setPen(QPen(QColor(color,0,0), 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
    painter.drawLine(x1,y1,x2,y2);
    painter.end();
}

void OKVC::Blit(int x1, int y1, int x2, int y2, int w, int h)
{
    int px1 = x1;
    int py1 = y1 + state.m_pram->get(p_srcPage)*16;
    int px2 = x2;
    int py2 = y2 + state.m_pram->get(p_dstPage)*16;
//#pragma omp parallel for
    for (int yy=0;yy<h;yy++)
        for (int xx=0;xx<w;xx++) {
            int sx = xx+px1;
            int sy = yy+py1;
            int tx = xx+px2;
            int ty = yy+py2;
            uchar c = 0;

            c = state.m_vram->get(sx+sy*256);
//            if (sy<256) // in Screen ram
            if (sx>=0 && sx<256 && sy>=0 && sy<256)
                c = m_img.pixelColor(sx,sy).red();

            state.m_vram->set(tx+ty*256,c);

//            if (ty<256) // in Screen ram
            if (tx>=0 && tx<256 && ty>=0 && ty<256)
                m_img.setPixelColor(tx,ty,QColor(c,0,0));
        }
}

void OKVC::Rect(int x1, int y1, int w, int h,uchar c)
{
    int px1 = x1;
    int py1 = y1;
    for (int yy=0;yy<h;yy++)
        for (int xx=0;xx<w;xx++) {
            int sx = xx+px1;
            int sy = yy+py1;

//            state.m_vram->set(tx+ty*256,c);

//            if (ty<256) // in Screen ram
  //          if (tx>=0 && tx<256 && ty>=0 && ty<256)
                m_img.setPixelColor(sx,sy,QColor(c,0,0));
        }
}

void OKVC::BlitFont(int fontsizeX, int fontsizeY, int chr, int col, int px, int py)
{
    int base = (state.m_pram->get(p_fontBank))*0x10000;
    int width = 256/fontsizeX;
    int x = chr % (width);
    int y = (int)(chr/width);
    int px1 = x*fontsizeX;
    int py1 = y*fontsizeY;
    int px2 = px;
    int py2 = py;
//    qDebug() << "Base: "<<QString::number(base,16);
//#pragma omp parallel for
    for (int yy=0;yy<fontsizeY;yy++)
        for (int xx=0;xx<fontsizeX;xx++) {
            int sx = xx+px1;
            int sy = yy+py1;

            int tx = xx+px2;
            int ty = yy+py2;
            uchar c = 0;

            c = state.m_vram->get(base + sx+sy*256);


            if (c!=0) {
                state.m_vram->set(tx+ty*256,col);

//            if (ty<256) // in Screen ram
                if (tx>=0 && tx<256 && ty>=0 && ty<256)
                    m_img.setPixelColor(tx,ty,QColor(col,0,0));
            }
        }

}



void OKVC::Update()
{
//    StripPramToVram();

    state.m_pram->set(p_time,rand()%256);
    if (get(p_exec)==p_pixel) {
        PutPixel(get(p_p1_x), get(p_p1_y),get(p_p1_c));
    }
    if (get(p_exec)==p_line) {
        DrawLine(get(p_p1_x), get(p_p1_y),get(p_p2_x), get(p_p2_y),    get(p_p1_c));
    }
    if (get(p_exec)==p_clearscreen) {
        m_img.fill(QColor(get(p_p1_c),0,0));
    }
    if (get(p_exec)==p_circlefill) {
        DrawCircle(get(p_p1_x), get(p_p1_y),get(p_p1_c),get(p_p1_3),true);
    }
    if (get(p_exec)==p_palette) {
        state.m_palette[get(p_p1_x)] = QColor(get(p_p1_y), get(p_p1_c), get(p_p1_3));
    }
    if (get(p_exec)==p_blit) {
        Blit(get(p_p1_x), get(p_p1_y), get(p_p1_c), get(p_p1_3),get(p_p2_x),get(p_p2_y));
    }
    if (get(p_exec)==p_blitFont) {
        BlitFont(get(p_fontSizeX), get(p_fontSizeY), get(p_p1_x), get(p_p1_y),get(p_p1_c),get(p_p1_3));
    }
    if (get(p_exec)==p_rect) {
        Rect(get(p_p1_x), get(p_p1_y), get(p_p1_c),get(p_p1_3),get(p_p2_x) );
    }
    if (get(p_exec)==p_drawPoly) {
        DrawPolygon(get(p_p1_x), get(p_p1_y), get(p_p1_c),get(p_p1_3),get(p_p2_x),get(p_p2_y),get(p_p2_c),true);
    }
    if (get(p_exec)==p_resetFileList)
        ResetFileList();

    if (get(p_exec)==p_nextFile)
        ReadNextFile();

    if (get(p_exec)==p_loadFile)
        LoadFile();

    set(p_exec,0);

    state.m_waitForVSYNC = (get(p_vsync)==1);

  //  StripVramToPram();

}

void OKVC::VRAMtoScreen()
{
//    QRgb* s =
    for (int y=0;y<256;y++)
        for (int x=0;x<256;x++)
            m_img.setPixelColor(x,y,QColor(state.m_vram->get(x+y*256),0,0));

}
QVector3D OKVC::Distort(QVector3D coord)
{
    float rsq = coord.x() * coord.x() + coord.y() * coord.y();
    coord += coord * (m_CD * rsq);
    coord *= m_barrelScale;
    return coord;
}

void OKVC::GenerateOutputSignal()
{
    int bw  = state.m_pram->get(p_borderWidth);
    int bh  = state.m_pram->get(p_borderHeight);
    int bc  = state.m_pram->get(p_borderColor);

//    qDebug() << "BorderColor : "<<bc << state.m_palette[bc];
    /*   for (int y=0;y<m_screen.height();y++)
//#pragma omp parallel for
        for (int x=0;x<m_screen.width();x++) {
            int col = m_img.pixelColor(x,y).red();
            if (y<bh || y>m_screen.height()-bh)
                col = bc;
            if (x<bw || x>m_screen.width()-bw)
                col = bc;
            m_screen.setPixelColor(x,y,state.m_palette[col]);
        }
        */

    QRgb *screen = (QRgb *) m_screen.bits();
    QRgb *back = (QRgb *) m_img.bits();
    int w = m_screen.width();

    for (int y=0;y<m_screen.height();y++)
//        #pragma omp parallel for
        for (int x=0;x<w;x++) {
            int pos = y*w+x;

            int col = QColor(back[pos]).red();
            if (y<bh || y>m_screen.height()-bh)
                col = bc;
            if (x<bw || x>w-bw)
                col = bc;
            screen[y*w+x] = state.m_palette[col].rgba();
        }
}

uchar OKVC::get(uint i)
{
    return state.m_pram->get(i);;
}

uchar OKVC::set(uint i, uchar j)
{
    state.m_pram->set(i,j);
    return j;
}
