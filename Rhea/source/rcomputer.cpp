#include "rcomputer.h"

RComputer::RComputer()
{

}

void RComputer::Step()
{
    m_cpu.Eat();
    m_rvc.Update();
}

void RComputer::PowerOn()
{
    m_cpu.Initialize();
    m_cpu.LoadOpcodes();
    m_rvc.Init(&m_cpu.m.m_data);
}

void RComputer::run()
{
    while (!m_abort) {

        m_outPut = m_outPut.fromImage(m_rvc.m_img);
        emit emitOutput();
        msleep(25);

    }
}
