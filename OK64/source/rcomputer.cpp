#include "rcomputer.h"
RComputer::RComputer()
{
    m_audio.Init();
    m_sid.reset();
    m_sid.set_sampling_parameters(2000000,SAMPLE_INTERPOLATE,44100);
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
        emit emitAudio();

        QElapsedTimer timer;
        m_rvc.PrepareRaster();
        timer.start();
        if (m_run) {
            m_cpu.ClearCycles();
            while (m_cpu.m_cycles< m_cyclesPerFrame && m_rvc.m_waitForVSYNC==false && m_run)
                Step();

        }
        m_workLoad = m_cpu.m_cycles/(m_cyclesPerFrame)*100.0;
        m_outPut = m_outPut.fromImage(m_rvc.m_img);
        emit emitOutput();
        usleep(1000000/m_fps - timer.nsecsElapsed()/1000.0);
        m_time++;
    }
}

void RComputer::onAudio()
{
    for (int i=0;i<0x20;i++)
        m_sid.write(i,m_cpu.m.m_data[0xD400+i]);

    int s = m_audio.m_size;///50;
    m_audio.m_cur=0;
//    cycle_count csdelta = round((float)2000000 / ((float)44100 / 32));
    cycle_count csdelta = round((float)1000000 / ((float)44100 / 1));
//    qDebug()<< csdelta;

//    m_sid.clock(csdelta,buf,32);
    for (int i=0;i<s;i++) {
//        qDebug() << v;
  //      short v = buf[i];
        m_sid.clock(csdelta);
        int v = m_sid.output();
//        qDebug() << v;
//        QByteArray* byteBuffer = &(m_audio.m_soundBuffer);
        float sample = (float)v/10240.0;

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
    m_audio.m_cur+=1;
}

void RAudio::Init() {
    qreal sampleRate = 44100;   // sample rate
    qreal duration = 1.000/50.0;     // duration in seconds
    int n  = duration * sampleRate;   // number of data samples

    m_size = n;
    m_soundBuffer.resize(n*4);
    m_soundBuffer.fill(0);
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
      m_input = new QBuffer(&m_soundBuffer);
      m_input->open(QIODevice::ReadOnly);   // set the QIODevice to read-only

      audio->start(m_input);
/*      connect(audio, &QAudioOutput::stateChanged, [this->audio, m_input](QAudio::State newState)
      {
                if (newState == QAudio::IdleState)   // finished playing (i.e., no more data)
               {
//                    audio->start(m_input);
               }
          // should also handle more states, e.g., errors. I need to figure out on how to do this later.
      });
*/
      connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
}

void RAudio::Audio() {



    //if (isPlaying)
     //   return;







/*    QBuffer* m_input;
    m_input = new QBuffer(&m_soundBuffer);
    m_input->open(QIODevice::ReadOnly);   // set the QIODevice to read-only

    // Create an audio output with our QAudioFormat
    QAudioOutput* audio = new QAudioOutput(audioFormat, this);

    // connect up signal stateChanged to a lambda to get feedback
    connect(audio, &QAudioOutput::stateChanged, [audio, m_input](QAudio::State newState)
    {
              if (newState == QAudio::IdleState)   // finished playing (i.e., no more data)
             {
     //            qDebug() << "finished playing sound";
                 delete audio;
                 delete m_input;
                  //isPlaying = false;
                 //delete byteBuffer;  // I tried to delete byteBuffer pointer (because it may leak memories), but got compiler error. I need to figure this out later.
             }
        // should also handle more states, e.g., errors. I need to figure out on how to do this later.
    });
//    isPlaying = true;
    audio->start(m_input);}
    */
}

void RAudio::handleStateChanged(QAudio::State newState)
{
//    switch (newState) {
  //  case QAudio::IdleState: // Finished playing (no more data)
//    qDebug() << "Something else: " << newState;
    if (newState==QAudio::IdleState) {
/*        delete m_input;
        m_input = new QBuffer(&m_soundBuffer);
        m_input->open(QIODevice::ReadOnly);   // set the QIODevice to read-only*/
        m_input->reset();
//        qDebug() << "HERE";
//
//        audio->start(m_input);
    }
/*        if(stop){
            audio->stop();
            qDebug() << "Stopped audio" << newState;
            delete audio;
            delete b;
            delete buf;
        }
        else{ // restart from scratch
            QDataStream s(buf, QIODevice::ReadWrite);
            audio->start(s.device());
        }
        break;

    case QAudio::StoppedState: // Stopped for other reasons
        if (audio->error() != QAudio::NoError) { // Error handling
            qDebug() << "Audio error: " << newState;
        }
        break;
     */
    /*default:
        // ... other cases as appropriate
        qDebug() << "Something else: " << newState;
        break;
    }
    */
}
