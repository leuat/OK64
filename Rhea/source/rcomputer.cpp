#include "rcomputer.h"

RComputer::RComputer()
{

}

void RComputer::Step()
{
    m_cpu.Eat();
    m_rvc.Update();
}

void RComputer::Run()
{
    m_run = true;
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
        if (m_run)

          for (int i=0;i<150000; i++)
            Step();

        m_outPut = m_outPut.fromImage(m_rvc.m_img);
        emit emitOutput();
        msleep(14);

    }
}
