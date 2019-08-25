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

        QElapsedTimer timer;
        m_rvc.PrepareRaster();
        timer.start();
        if (m_run) {
            m_cpu.ClearCycles();
            while (m_cpu.m_cycles< 320*320 && m_rvc.m_waitForVSYNC==false)
                Step();

        }
        m_workLoad = m_cpu.m_cycles/(320.0*320.0)*100.0;
        m_outPut = m_outPut.fromImage(m_rvc.m_img);
        emit emitOutput();
        usleep(20000 - timer.nsecsElapsed()/1000.0);
    }
}
