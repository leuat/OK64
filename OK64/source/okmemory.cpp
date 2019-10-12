#include "okmemory.h"
#include <QDebug>
void OKMemory::Init(int size)
{
    m_memory.resize(size);
    m_memory.fill(0);
}

uchar OKMemory::get(int pos)
{
    if (pos>=m_memory.count())
        return 0;
    return m_memory[pos];
}

ushort OKMemory::getWord(int pos)
{
    if (pos>=m_memory.count())
        return 0;
    return (m_memory[pos]<<8) | m_memory[pos+1];
}


void OKMemory::set(int pos, uchar v)
{
//    qDebug() << " Setting " << QString::number(pos,16) << QString::number(v);
    if (pos>=m_memory.count())
        return;
    m_memory[pos] = v;

}

OKMemory::OKMemory()
{

}
