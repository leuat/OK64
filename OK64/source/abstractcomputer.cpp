#include "abstractcomputer.h"



QByteArray RAudio::m_soundBuffer;
int RAudio::m_currentPos = 0;
qint64 RAudio::m_soundPos = 0;

void RAudio::Init(int samplerate, float dur) {
    if (m_type==TYPE_DISABLED)
        return;

    float sampleRate = samplerate;   // sample rate
    float duration = dur;     // duration in seconds
    int n  = int(duration * sampleRate);   // number of data samples
    //    m_soundPos = 0;
    m_size = n;
    m_soundPos = 0;
    m_soundBuffer.resize(n*4*m_bufscale);
    m_soundBuffer.fill(0);


    /* Set the audio format */
    wanted.freq = sampleRate;
    wanted.format = AUDIO_S16;
    wanted.channels = 2;    /* 1 = mono, 2 = stereo */
    wanted.samples = 1024*4;  /* Good low-latency value for callback */
    wanted.callback = fill_audio_sdl;
    wanted.userdata = NULL;


    /* Open the audio device, forcing the desired format */
    if ( SDL_OpenAudio(&wanted, NULL) < 0 ) {
        qDebug() << "Couldn't open audio: ";
        qDebug() << SDL_GetError();
        m_type = TYPE_DISABLED;
    }
    SDL_PauseAudio(0);
}

void RAudio::fill_audio_sdl(void *udata, Uint8 *stream, int len)
{

    int size = m_soundBuffer.size();
    int p = (m_currentPos+size-4096*4)%size;
    SDL_memset(stream, 0, len);
    unsigned char tmp[100000];
    for (int i=0;i<len;i++)
        tmp[i] = m_soundBuffer[p+i];
    SDL_MixAudio(stream, tmp, len, SDL_MIX_MAXVOLUME);
    m_currentPos+=len;

}

RAudio::~RAudio()
{
    if (m_type==TYPE_SDL)
        SDL_CloseAudio();
}




AbstractComputer::AbstractComputer()
{

    m_audio.Init(m_khz,1.00/m_fps);
    m_sid.reset();
#ifdef __linux__
//   m_sid.set_sampling_parameters(m_mhz,SAMPLE_FAST,m_khz);
#endif
    connect(this,SIGNAL(emitAudio()),this,SLOT(onAudio()));

}



void AbstractComputer::Step()
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

void AbstractComputer::Run()
{
    m_run = true;
}


void AbstractComputer::PowerOn()
{
    m_pram.Init(65536+0x100); // fucked up thing... 256 bytes off. WTF?
    m_vram.Init(65536*16);
    m_cpu.Initialize(&m_pram);
    m_cpu.LoadOpcodes();
    m_okvc.Init(&m_pram, &m_vram, m_cpu.m_impl);

}

void AbstractComputer::Reset()
{
    m_run=false;
    m_cpu.Initialize(&m_pram);
    m_okvc.Init(&m_pram, &m_vram, m_cpu.m_impl);
    //    m_audio.Init(m_khz,1.0/m_fps);
    m_audio.m_reset = 1;
    m_audio.m_soundBuffer.fill(0);
    m_run = true;

}

int AbstractComputer::LoadProgram(QString fn)
{
    if (!QFile::exists(fn))
        return 0;
    Reset();
    m_okvc.LoadRom(fn,0,true);
    m_cpu.m_impl->pc = 0x400;
    m_okvc.VRAMtoScreen();
    QString symFile = fn.remove(".prg") + ".sym";
    if (QFile::exists(symFile))
        m_cpu.LoadSybols(symFile);

    return 0;
}

void AbstractComputer::HandleInput()
{

    if (m_currentKey!=0) {
        m_pram.set(m_okvc.p_inputKey,m_currentKey);
        m_currentKey=0;
        if (m_okvc.InputVectorSet()) {
            m_cpu.m_impl->INP();
        }
    }
}
void AbstractComputer::run()
{
    Execute();

}

void AbstractComputer::Execute()
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

void AbstractComputer::onAudio()
{
    if (m_audio.m_type==RAudio::TYPE_DISABLED)
        return;
    if (m_abort)
        return;

    m_audioAction=true;

    for (int i=0;i<0x20;i++)
        m_sid.write(i,m_pram.get(m_okvc.p_sid+i));

    cycle_count csdelta = 1.0*((float)m_mhz / ((float)m_khz));
    int size = m_audio.m_soundBuffer.size();

    if (m_audio.m_reset==1) {
        m_audio.m_reset = 0;
        m_audio.m_currentPos = 0;
        m_audio.m_soundPos = 0;
    }

    for (int i=0;i<m_audio.m_size;i++) {

        m_sid.clock(csdelta);
        int c1 = m_sid.output()*m_pram.get(m_okvc.p_channel1Vol)/255.0;
        int c2 = m_sid.output()*m_pram.get(m_okvc.p_channel2Vol)/255.0;

        char *ptr1 = (char*)(&c1);
        char *ptr2 = (char*)(&c2);
        int j = (size+ i*m_bpp + m_audio.m_soundPos)%(size);
        m_audio.m_soundBuffer[j+0] = *ptr1;
        m_audio.m_soundBuffer[(j+1)%size] = *(ptr1 + 1);
        m_audio.m_soundBuffer[(j+2)%size] = *ptr2;
        m_audio.m_soundBuffer[(j+3)%size] = *(ptr2 + 1);
    }
    m_audio.m_soundPos=(m_audio.m_soundPos + m_bpp*m_audio.m_size)%size;
    m_audioAction=false;

}

