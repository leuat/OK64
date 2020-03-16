#include "rcomputer.h"

RComputer::RComputer()
{
    m_audio.Init(m_khz,1.0/m_fps);
    m_sid.reset();
#ifdef __linux__
//   m_sid.set_sampling_parameters(m_mhz,SAMPLE_FAST,m_khz);
#endif
//    m_sid.
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
        if (u == m_cpu.r.pc) {
            m_run = false;
            m_cpu.m_breakPoints.removeAll(u);
            return;

        }
        i++;
    }

    if (!m_cpu.Eat()) {
        m_run = false;
    }

    m_okvc.Update();


}

void RComputer::Run()
{
    m_run = true;
    m_audio.m_input->seek(0);
}


void RComputer::PowerOn()
{
    m_pram.Init(65536+0x100); // fucked up thing... 256 bytes off. WTF?
    m_vram.Init(65536*16);
    m_cpu.Initialize(&m_pram);
    m_cpu.LoadOpcodes();
    m_okvc.Init(&m_pram, &m_vram, m_cpu.m_impl);

}

void RComputer::Reset()
{
    m_run=false;
    m_cpu.Initialize(&m_pram);
    m_okvc.Init(&m_pram, &m_vram, m_cpu.m_impl);
//    m_audio.Init(m_khz,1.0/m_fps);
    m_audio.m_reset = 1;
    m_run = true;

}

int RComputer::LoadProgram(QString fn)
{
    Reset();
    m_okvc.LoadRom(fn,0,true);
    m_cpu.m_impl->pc = 0x400;
    m_okvc.VRAMtoScreen();

    return 0;
}

void RComputer::HandleInput()
{

    if (m_currentKey!=0) {
        m_pram.set(m_okvc.p_inputKey,m_currentKey);
        m_currentKey=0;
        if (m_okvc.InputVectorSet()) {
            m_cpu.m_impl->INP();
        }
    }
}
void RComputer::run()
{
    Execute();

}

void RComputer::Execute()
{
    int time = 0;
    int cnt = 1;
    while (!m_abort) {
        QElapsedTimer timer;
        timer.start();
        HandleInput();
        m_okvc.PrepareRaster();
        if (m_run) {
            m_cpu.ClearCycles();
            while (m_cpu.m_cycles< m_cyclesPerFrame && m_okvc.state.m_waitForVSYNC==false && m_run && !m_abort)
                Step();

        }

        m_workLoad = m_cpu.m_cycles/((float)m_cyclesPerFrame)*100.0;
        emit emitAudio();

        m_time++;
        if (!m_abort) {
            time++;

            m_okvc.GenerateOutputSignal();
            if (!m_outputBusy)
                emit emitOutput();

            long slp = 1;
            cnt = 0;
            while (slp>0) {
                slp = (m_mhz/m_fps) -  (float)timer.nsecsElapsed()/1000.0;
                cnt++;
            }

        }


  }

    m_audio.done = true;
    //   qDebug() << "Thread DONE";
}

void RComputer::onAudio()
{
    if (m_abort)
        return;

    m_audioAction=true;

//    return;
    for (int i=0;i<0x20;i++)
        m_sid.write(i,m_pram.get(m_okvc.p_sid+i));

//    qDebug() << s;
    cycle_count csdelta = 1.0*((float)m_mhz / ((float)m_khz));
    int size = m_audio.m_size*m_bpp*(int)(m_audio.m_bufscale);
    if (m_audio.m_reset==1) {
        if (isFirst) {
            m_audio.m_input->seek(0);

            isFirst = false;
        }
        m_audio.m_soundPos = m_audio.m_input->pos()+m_bpp*m_audio.m_size*16;

        m_audio.m_reset = 0;
    }
    if (m_audio.m_reset==2) {
        m_audio.m_reset = 0;
    }
//    qDebug() << m_audio.m_input->pos() << " vs " << m_audio.m_soundPos;

    for (int i=0;i<m_audio.m_size;i++) {

        m_sid.clock(csdelta);
        int c1 = m_sid.output()*m_pram.get(m_okvc.p_channel1Vol)/255.0;
        int c2 = m_sid.output()*m_pram.get(m_okvc.p_channel2Vol)/255.0;
        char *ptr1 = (char*)(&c1);
        char *ptr2 = (char*)(&c2);
//        int j = (size+ i*m_bpp + m_audio.m_soundPos-m_bpp*44100)%(size);// + m_audio.m_cur*4*s;
        int j = (size+ i*m_bpp + m_audio.m_soundPos)%(size);// + m_audio.m_cur*4*s;
        m_audio.m_soundBuffer[j+0] = *ptr1;
        m_audio.m_soundBuffer[j+1] = *(ptr1 + 1);
        m_audio.m_soundBuffer[j+2] = *ptr2;
        m_audio.m_soundBuffer[j+3] = *(ptr2 + 1);
    }

    m_audio.m_soundPos=(m_audio.m_soundPos + m_bpp*m_audio.m_size)%size;
    m_audioAction=false;

}

void RAudio::CopyBuffer()
{
    for (int i=0;i<m_tempSoundBuffer.count();i++)
          m_tempSoundBuffer[i]=m_soundBuffer[i];

}

void RAudio::Init(int samplerate, float dur) {
    float sampleRate = samplerate;   // sample rate
    float duration = dur;     // duration in seconds
    int n  = int(duration * sampleRate);   // number of data samples
//    m_soundPos = 0;
    m_size = n;
    m_soundPos = 0;
    m_soundBuffer.resize(n*4*m_bufscale);
    m_soundBuffer.fill(0);
    m_tempSoundBuffer.resize(n*4*m_bufscale);
    m_tempSoundBuffer.fill(0);
    audioFormat.setSampleRate(static_cast<int>(sampleRate));
    audioFormat.setChannelCount(2);
    audioFormat.setSampleSize(16);   // set the sample size in bits. We set it to 32 bis, because we set SampleType to float (one float has 4 bytes ==> 32 bits)
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
//    audioFormat.setSampleType(QAudioFormat::Float);   // use Float, to have a better resolution than SignedInt or UnSignedInt
    audioFormat.setSampleType(QAudioFormat::SignedInt);   // use Float, to have a better resolution than SignedInt or UnSignedInt
    QAudioDeviceInfo deviceInfo(QAudioDeviceInfo::defaultOutputDevice());
    if(!deviceInfo.isFormatSupported(audioFormat))
    {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }
    if (audio==nullptr)
        audio = new QAudioOutput(audioFormat, this);
//    connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
//      m_input = new QInfiniteBuffer(&m_tempSoundBuffer,nullptr);
//      m_input = new QInfiniteBuffer(&m_soundBuffer,nullptr);
      m_input = new QBuffer(&m_soundBuffer,nullptr);
      m_input->open(QIODevice::ReadOnly);   // set the QIODevice to read-only

/*      m_aThread.audio = audio;
      m_aThread.buf = m_input;
      m_aThread.start();
      m_aThread.run();*/

      audio->reset();
      audio->start(m_input);
      connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
}


void RAudio::handleStateChanged(QAudio::State newState)
{
    if (newState==QAudio::StoppedState) {
//        qDebug() << "STOPPED";
    }

    if (newState==QAudio::IdleState && !done) {
//            qDebug()<< "IDLE";
 //         audio->reset();
//            audio->setNotifyInterval(10000);
//            audio->
          m_input->seek(0);
          audio->start(m_input);
          m_reset = 2;
    }
    if (done) {
  //      qDebug() << "DONE";
        audio->stop();

        delete m_input;
        m_input = nullptr;
    }
}

qint64 QInfiniteBuffer::readData(char *output, qint64 maxlen)
{
    qint64 outputpos=0;
       const QByteArray &d=data();

       do
       {
           qint64 sizetocopy=maxlen-outputpos;
           if((maxlen-outputpos)>(d.size()-m_pos))
               sizetocopy=d.size()-m_pos;

           memcpy(output,d.constData()+m_pos,sizetocopy);
           outputpos+=sizetocopy;
           m_pos=(m_pos+sizetocopy)%d.size();
 //          if(m_pos>=d.size())
   //            m_pos=0;
       } while(outputpos<maxlen);

       return maxlen;
}

