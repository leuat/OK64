#include "rcomputer.h"

RComputer::RComputer()
{
    m_audio.Init(m_khz,1.0/m_fps);
    m_sid.reset();
    m_sid.set_sampling_parameters(m_mhz,SAMPLE_INTERPOLATE,m_khz);
    connect(this,SIGNAL(emitAudio()),this,SLOT(onAudio()));
/*    bool set_sampling_parameters(double clock_freq, sampling_method method,
                     double sample_freq, double pass_freq = -1,
                     double filter_scale = 0.97);
*/
}

void RComputer::Step()
{
    int i=0;
    for (ushort u:m_cpu.m_breakPoints) {
//        qDebug() << "BP should be " << Util::numToHex(u);
        if (u == m_cpu.r.pc) {
            m_run = false;
            m_cpu.m_breakPoints.removeAll(u);
  //          qDebug() << "BP " << Util::numToHex(m_cpu.r.pc);
            //        m_cpu.m_breakPoints.remove( m_cpu.m_breakPoints.at(m_cpu.r.pc));
            return;

        }
        i++;
    }

    if (!m_cpu.Eat()) {
        m_run = false;
    }
  //  m_cpu.Eat();
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
            while (m_cpu.m_cycles< m_cyclesPerFrame && m_rvc.m_waitForVSYNC==false && m_run)
                Step();

//            if (m_cpu.m_cycles>= m_cyclesPerFrame)
//                qDebug() << "CAP at" << m_cpu.m_cycles << " of " <<m_cyclesPerFrame;

        }
        m_workLoad = m_cpu.m_cycles/((float)m_cyclesPerFrame)*100.0;

        m_time++;
        usleep(m_mhz/(float)m_fps - (float)timer.nsecsElapsed()/1000.0);
//        qDebug() << timer.nsecsElapsed()/1000.0;
        emit emitOutput();
        emit emitAudio();

    }
    m_audio.done = true;
}

void RComputer::onAudio()
{
    for (int i=0;i<0x20;i++)
        m_sid.write(i,m_cpu.m.m_data[0xD400+i]);

    int s = m_audio.m_size;///50;
    cycle_count csdelta = round((float)m_mhz / ((float)m_khz));
    for (int i=0;i<s;i++) {
        m_sid.clock(csdelta);
        int v = m_sid.output();
        float sample = (float)v/65536.0;
//        sample = sin((m_time+i)*0.1)*0.1;
        char *ptr = (char*)(&sample);  // assign a char* pointer to the address of this data sample
        char byte00 = *ptr;         // 1st byte
        char byte01 = *(ptr + 1);   // 2nd byte
        char byte02 = *(ptr + 2);   // 3rd byte
        char byte03 = *(ptr + 3);   // 4th byte
        int j = i*4;
        m_audio.m_soundBuffer[j+0] = byte00;
        m_audio.m_soundBuffer[j+1] = byte01;
        m_audio.m_soundBuffer[j+2] = byte02;
        m_audio.m_soundBuffer[j+3] = byte03;
    }
}

void RAudio::Init(int samplerate, float dur) {
    float sampleRate = samplerate;   // sample rate
    float duration = dur;     // duration in seconds
    int n  = duration * sampleRate;   // number of data samples

    m_size = n;
    m_soundBuffer.resize(n*4);
    m_soundBuffer.fill(0);
    m_tempSoundBuffer.resize(n*4);
    m_tempSoundBuffer.fill(0);
    audioFormat.setSampleRate(static_cast<int>(sampleRate));
    audioFormat.setChannelCount(1);
    audioFormat.setSampleSize(32);   // set the sample size in bits. We set it to 32 bis, because we set SampleType to float (one float has 4 bytes ==> 32 bits)
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::Float);   // use Float, to have a better resolution than SignedInt or UnSignedInt
    QAudioDeviceInfo deviceInfo(QAudioDeviceInfo::defaultOutputDevice());
    if(!deviceInfo.isFormatSupported(audioFormat))
    {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    audio = new QAudioOutput(audioFormat, this);
      connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
      m_input = new QInfiniteBuffer(&m_soundBuffer,nullptr);
      m_input->open(QIODevice::ReadOnly);   // set the QIODevice to read-only

      audio->start(m_input);
      connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
}


void RAudio::handleStateChanged(QAudio::State newState)
{

    if (newState==QAudio::IdleState && !done) {
//        m_input->reset();
//        m_soundBuffer.setRawData(m_tempSoundBuffer.ch);
        for (int i=0;i<m_tempSoundBuffer.count();i++) {
            m_soundBuffer[i]=m_tempSoundBuffer[i];
        }
        m_input->seek(0);
//        m_input->start
        //audio->start(m_input);
    }
    if (done) {
        audio->stop();
        delete m_input;
        m_input = nullptr;
    }
}
