#include "rcomputer.h"
RComputer::RComputer()
{
    m_audio.Init();
    connect(this,SIGNAL(emitAudio()),this,SLOT(onAudio()));

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

        }
        emit emitAudio();
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

//    for (int i=0;i<m_audio.m_soundBuffer.count()/4;i++) {
    for (int i=0;i<m_audio.m_size;i++) {
//        qDebug() << v;
        m_sid.clock();
        unsigned int v = m_sid.output();

//        QByteArray* byteBuffer = &(m_audio.m_soundBuffer);
        float sample = (float)v/10200.0;  // save one data sample in a local variable, so I can break it down into four bytes
//        qDebug() << sample;
//        sample = 1+sin((m_time*0.1 ))*1.0;// + sin(i*0.002);
//        sample = 1+sin(i*0.002);
        char *ptr = (char*)(&sample);  // assign a char* pointer to the address of this data sample
        char byte00 = *ptr;         // 1st byte
        char byte01 = *(ptr + 1);   // 2nd byte
        char byte02 = *(ptr + 2);   // 3rd byte
        char byte03 = *(ptr + 3);   // 4th byte
//        int j = i*4 + m_audio.m_cur;
        int j = i*4;
        m_audio.m_soundBuffer[j] = byte00;
        m_audio.m_soundBuffer[j+1] = byte01;
        m_audio.m_soundBuffer[j+2] = byte02;
        m_audio.m_soundBuffer[j+3] = byte03;
    }
    exit(1);
//    m_audio.m_cur+=m_audio.m_size*4;
    m_audio.Audio();
}

void RAudio::Init() {
    qreal sampleRate = 40000;   // sample rate
    qreal duration = 1.000/50;     // duration in seconds
    const quint32 n = static_cast<quint32>(duration * sampleRate);   // number of data samples

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

}

void RAudio::Audio() {
    //if (isPlaying)
     //   return;







    QBuffer* m_input;
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
