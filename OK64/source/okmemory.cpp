#include "okmemory.h"

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

void OKMemory::set(int pos, uchar v)
{
    if (pos>=m_memory.count())
        return;
    m_memory[pos] = v;

}

OKMemory::OKMemory()
{

}
