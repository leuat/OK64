#include "rcomputer.h"

RComputer::RComputer()
{
    m_audio.Init(m_khz,1.0/m_fps);
    m_sid.reset();
    //m_sid.set_sampling_parameters(m_mhz,SAMPLE_FAST,m_khz);

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
    m_okvc.Update();


}

void RComputer::Run()
{
    m_run = true;
    m_audio.m_input->seek(0);
}


void RComputer::PowerOn()
{
    m_pram.Init(65536);
    m_vram.Init(65536*16);
    m_cpu.Initialize(&m_pram);
    m_cpu.LoadOpcodes();
    m_okvc.Init(&m_pram, &m_vram);
}

int RComputer::LoadProgram(QString fn)
{
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly)) return 0;
    QByteArray blob = file.readAll();
    int pos = blob[1]*0x100 + blob[0];
    //qDebug() << blob.size();

    int programSize = min(blob.size()+pos,65536);
    blob.remove(0,2);
    for (int i=0;i<programSize;i++)
        if (i+pos<0xFF00 || i+pos>0xFFFF)
        m_pram.set(i+pos,blob[i]);

    blob.remove(0,programSize);
//    qDebug() << blob.size();
    if (blob.size()>0) {
  //      qDebug() << "VRAM size " << blob.size();
        for (int i=0;i<blob.size();i++)
            m_vram.set(i,blob[i]);


    }
    m_okvc.VRAMtoScreen();

    return pos;
}

void RComputer::HandleInput()
{
    if (m_currentKey!=0) {
        qDebug() << "Key pressed:  " << m_currentKey;
        m_pram.set(m_okvc.p_inputKey,m_currentKey);
        m_currentKey=0;
        m_cpu.m_impl->INP();
    }
}
void RComputer::run()
{
    Execute();

}

void RComputer::Execute()
{
    sleep((1/100.0));
    onAudio();
    int time = 0;
    while (!m_abort) {
        HandleInput();
        QElapsedTimer timer;
        timer.start();
        m_okvc.PrepareRaster();
        if (m_run) {
            m_cpu.ClearCycles();
            while (m_cpu.m_cycles< m_cyclesPerFrame && m_okvc.state.m_waitForVSYNC==false && m_run && !m_abort)
                Step();

        }

        m_workLoad = m_cpu.m_cycles/((float)m_cyclesPerFrame)*100.0;

        m_time++;
        //m_okvc.m_backbuffer = QImage(m_okvc.m_img);
        if (!m_abort) {
            time++;
            if ((time&1)==1)
                m_okvc.GenerateOutputSignal();

            m_soundPos = ((QInfiniteBuffer*)m_audio.m_input)->getPos();
            onAudio();
            int slp = m_mhz/(float)m_fps - (float)timer.nsecsElapsed()/1000.0;
            if (slp>0)
                usleep(slp);
            else qDebug() << "frame skip at "<<time ;

            if ((time&1)==0) {
                if (!m_outputBusy) {
                    emit emitOutput();
                }
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
        m_sid.write(i,m_pram.get(0xD400+i));
//    qDebug() << s;
    cycle_count csdelta = round((float)m_mhz / ((float)m_khz));
//    int pp = m_audio.m_input->pos();
//    qDebug() << pp;
//#pragma omp parallel for
    for (int i=0;i<m_audio.m_size;i++) {
//        if (m_abort)
  //          return;

        m_sid.clock(csdelta);
        int v = m_sid.output();
        float sample = (float)v/65536.0;
//        sample = sin((i)*(0.1*(sin(m_time/10.0)+1)))*0.2;
        char *ptr = (char*)(&sample);  // assign a char* pointer to the address of this data sample
        char byte00 = *ptr;         // 1st byte
        char byte01 = *(ptr + 1);   // 2nd byte
        char byte02 = *(ptr + 2);   // 3rd byte
        char byte03 = *(ptr + 3);   // 4th byte
        int j = (m_audio.m_size*4+ i*4 + m_soundPos)%(m_audio.m_size*4);// + m_audio.m_cur*4*s;
        m_audio.m_soundBuffer[j+0] = byte00;
        m_audio.m_soundBuffer[j+1] = byte01;
        m_audio.m_soundBuffer[j+2] = byte02;
        m_audio.m_soundBuffer[j+3] = byte03;
    }

/*    if (m_audio.m_cur++==m_audio.m_bufscale) {
        m_audio.m_cur = 0;
    }
*/
//    m_audio.CopyBuffer();

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

    m_size = n*m_bufscale;
    m_soundBuffer.resize(n*4*m_bufscale);
    m_soundBuffer.fill(0);
    m_tempSoundBuffer.resize(n*4*m_bufscale);
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
//    connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
//      m_input = new QInfiniteBuffer(&m_tempSoundBuffer,nullptr);
      m_input = new QInfiniteBuffer(&m_soundBuffer,nullptr);
      m_input->open(QIODevice::ReadOnly);   // set the QIODevice to read-only

/*      m_aThread.audio = audio;
      m_aThread.buf = m_input;
      m_aThread.start();
      m_aThread.run();*/
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
//          audio->reset();
//            audio->setNotifyInterval(10000);
//            audio->
   //       m_input->seek(0);
  //        audio->start(m_input);
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
           m_pos+=sizetocopy;
           if(m_pos>=d.size())
               m_pos=0;
       } while(outputpos<maxlen);

       return maxlen;
}

