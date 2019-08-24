#include "mos6502.h"

Mos6502::Mos6502()
{

}

void Mos6502::Initialize()
{
    m.m_data.resize(65536);
}

void Mos6502::Eat()
{
    uchar instruction = m.m_data[r.pc];

}
