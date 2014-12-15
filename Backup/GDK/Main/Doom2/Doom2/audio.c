#include "common.h"
#include "mapp.h"

// Disable __cplusplus if defined to ensure we can use the c functions
#ifdef __cplusplus
#undef __cplusplus
#endif

#include <initguid.h>
//#include <xaudio2.h>
#include "audiomidi.h"
#include "audiop.h"

BOOL      g_comInitialized = FALSE;
//IXAudio2* g_IXAudio2 = NULL;
//IXAudio2MasteringVoice* g_IXAudio2MasteringVoice = NULL;
float g_soundVolume = 1.0f;

char* g_doomMusic[] =
{
"D_RUNNIN",
"D_STALKS",
"D_COUNTD",
"D_BETWEE",
"D_DOOM",
"D_THE_DA",
"D_SHAWN",
"D_DDTBLU",
"D_IN_CIT",
"D_DEAD",
"D_STLKS2",
"D_THEDA2",
"D_DOOM2",
"D_DDTBL2",
"D_RUNNI2",
"D_DEAD2",
"D_STLKS3",
"D_ROMERO",
"D_SHAWN2",
"D_MESSAG",
"D_COUNT2",
"D_DDTBL3",
"D_AMPIE",
"D_THEDA3",
"D_ADRIAN",
"D_MESSG2",
"D_ROMER2",
"D_TENSE",
"D_SHAWN3",
"D_OPENIN",
"D_EVIL",
"D_ULTIMA",
"D_READ_M",
"D_DM2TTL",
"D_DM2INT",
};

BOOL AudioInitialize()
{
    HRESULT hr = S_OK;
    unsigned int flags = 0;

#ifndef NDEBUG
//    flags |= XAUDIO2_DEBUG_ENGINE;
#endif

  //  hr = CoInitialize(NULL);
    if (SUCCEEDED(hr))
    {
        g_comInitialized = TRUE;
    }
    else
    {
        DebugOut("Failed to initialize COM, hr = 0x%lx", hr);
    }

    if (SUCCEEDED(hr))
    {
    //    hr = XAudio2Create(&g_IXAudio2, flags, XAUDIO2_DEFAULT_PROCESSOR);
        if (FAILED(hr))
        {
            DebugOut("Failed to create XAudio2, hr = 0x%lx", hr);
        }
    }

    if (SUCCEEDED(hr))
    {
      //  hr =  IXAudio2_CreateMasteringVoice(g_IXAudio2, &g_IXAudio2MasteringVoice, 0,0,0,0, NULL );
        if (FAILED(hr))
        {
            DebugOut("Failed to create XAudio2 Mastering voice, hr = 0x%lx", hr);
        }
    }

    if (SUCCEEDED(hr))
    {
        // Ignoring failure here on purpose.  Not sure where default volume settings belong
        //IXAudio2MasteringVoice_SetVolume(g_IXAudio2MasteringVoice, 0.25f, 0);
    }

    if (SUCCEEDED(hr))
    {
        // Initialize MIDI playback engine
        if (!AudioMidiInitialize())
        {
            hr = E_FAIL;
        }
    }

    if (SUCCEEDED(hr))
    {
        DebugOut("Audio initialized successfully");
    }
    else
    {
        // avoid invalid state by doing a targeted cleanup.  AudioUninitialize could have been
        // called, but this seems to fit better.
  //      if (g_IXAudio2)
        {
    //        IXAudio2_Release(g_IXAudio2);
      //      g_IXAudio2 = NULL;
        }

        //if (g_comInitialized)
        {
          //  CoUninitialize();
            g_comInitialized = FALSE;
        }

        DebugOut("Audio failed to initialize");
    }

    return (SUCCEEDED(hr));
}

void AudioUninitialize()
{
    AudioMidiUninitialize();

    //if (g_IXAudio2MasteringVoice)
    {
      //  IXAudio2MasteringVoice_DestroyVoice(g_IXAudio2MasteringVoice);
    }

    //if (g_IXAudio2)
    {
      //  IXAudio2_Release(g_IXAudio2);
    }

    //if (g_comInitialized)
    {
        //CoUninitialize();
        g_comInitialized = FALSE;
    }

    DebugOut("Audio uninitialized");
}

byte_t AudioSetMusicVolume(byte_t volume)
{
    return AudioMidiSetVolume(volume);
}

byte_t AudioSetSoundVolume(byte_t volume)
{
    g_soundVolume = (float)volume / 100.0f;
    return volume;
}

byte_t* AudioLoadSoundBuffer(const char* name)
{
    byte_t* soundMemory = NULL;
    long soundMemorySize = 0;
    byte_t* soundBufferMemory = NULL;
    sound_t* soundBuffer = NULL;
    byte_t miniHeaderOffset = sizeof(uint_t) + sizeof(void*);

    if (!WadGetLump(name, (void**)&soundMemory, &soundMemorySize))
    {
        DebugOut("Failed to load sound buffer '%s'", name);
    }
    else
    {
        soundBufferMemory = (byte_t*)MemoryAlloc("Sound Buffer", soundMemorySize + sizeof(uint_t) + sizeof(void*));

        soundBuffer = (sound_t*)soundBufferMemory;
        soundBuffer->datasize = soundMemorySize;
        soundBuffer->context = NULL;
        memcpy(soundBuffer->data, soundMemory, soundMemorySize);

        MemoryFree(soundMemory);
    }

    return soundBufferMemory;
}

void AudioFreeSound(byte_t* sound)
{
    sound_t* soundBuffer = NULL;
    //IXAudio2SourceVoice* voice = NULL;
    assert(sound);

    soundBuffer = (sound_t*)sound;
    //voice = (IXAudio2SourceVoice*)soundBuffer->context;

    //if (voice)
    {
        //IXAudio2SourceVoice_Stop(voice, 0, 0);
        //IXAudio2SourceVoice_FlushSourceBuffers(voice);
        //IXAudio2SourceVoice_DestroyVoice(voice);
    }

    MemoryFree(sound);
}

void AudioPlaySound(const char* name)
{
    byte_t* soundData = (byte_t*)ResourceManagerDataFromHandle(resource_type_soundbuffer, MapGetSound(name));
    sound_t* soundBuffer = (sound_t*)soundData;
    //IXAudio2SourceVoice* voice = (IXAudio2SourceVoice*)soundBuffer->context;
    HRESULT hr = S_OK;
    BOOL isRunning = TRUE;
    //XAUDIO2_VOICE_STATE state;

#if 0
    WAVEFORMATEX header = {0};

    XAUDIO2_BUFFER buffer = {0};

    header.cbSize = sizeof(header);
    header.wFormatTag = WAVE_FORMAT_PCM;
    header.nChannels = 1;
    header.nSamplesPerSec = 11025;
    header.wBitsPerSample = 8;
    header.nBlockAlign = header.nChannels * (header.wBitsPerSample / 8);
    header.nAvgBytesPerSec = header.nBlockAlign * header.nSamplesPerSec;

    //if (!g_IXAudio2)
    {
        DebugOut("Audio is not initialized");
        return;
    }

    if (!voice)
    {
        hr = IXAudio2_CreateSourceVoice(g_IXAudio2, &voice, &header, 0, 2.0f,0,0,NULL);
    }

    if (SUCCEEDED(hr))
    {
        soundBuffer->context = (void*)voice;

        buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
        buffer.AudioBytes = soundBuffer->datasize;
        buffer.pAudioData = soundBuffer->data;

        IXAudio2SourceVoice_GetState(voice, &state );
        isRunning = ( state.BuffersQueued > 0 ) != 0;

        // cancel currently running sound and play new one immediately
        if (isRunning)
        {
            IXAudio2SourceVoice_Stop(voice, 0, 0);
            IXAudio2SourceVoice_FlushSourceBuffers(voice);
            //return;
        }

        IXAudio2SourceVoice_SubmitSourceBuffer(voice, &buffer, NULL);
        IXAudio2SourceVoice_Start(voice, 0, 0);
        IXAudio2SourceVoice_SetVolume(voice, g_soundVolume, XAUDIO2_COMMIT_NOW);
    }
#endif
}

void AudioPlayMusic(const char* name)
{
    void* soundMemory = NULL;
    long soundMemorySize = 0;

    //if (!g_IXAudio2)
    {
      //  DebugOut("Audio is not initialized");
        //return;
    }

    if (WadGetLump(name, &soundMemory, &soundMemorySize))
    {
        AudioPlayMusData(soundMemory, soundMemorySize);
        MemoryFree(soundMemory);
    }
}

void AudioHandleMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
{
    AudioMidiHandleMessage(Msg, wParam, lParam);
}

void DebugAudioPlaySound(const char* name)
{
    void* soundMemory = NULL;
    long soundMemorySize = 0;
    //IXAudio2SourceVoice* voice = NULL;
    HRESULT hr = S_OK;
    BOOL isRunning = TRUE;
    //XAUDIO2_VOICE_STATE state;

#if 0
    WAVEFORMATEX header = {0};

    XAUDIO2_BUFFER buffer = {0};

    header.cbSize = sizeof(header);
    header.wFormatTag = WAVE_FORMAT_PCM;
    header.nChannels = 1;
    header.nSamplesPerSec = 11025;
    header.wBitsPerSample = 8;
    header.nBlockAlign = header.nChannels * (header.wBitsPerSample / 8);
    header.nAvgBytesPerSec = header.nBlockAlign * header.nSamplesPerSec;

    if (!g_IXAudio2)
    {
        DebugOut("Audio is not initialized");
        return;
    }

    if (WadGetLump(name, &soundMemory, &soundMemorySize))
    {
        hr = IXAudio2_CreateSourceVoice(g_IXAudio2, &voice, &header, 0, 2.0f,0,0,NULL);
        if (SUCCEEDED(hr))
        {
            buffer.pAudioData = (byte_t*)soundMemory;
            buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
            buffer.AudioBytes = soundMemorySize;

            IXAudio2SourceVoice_SubmitSourceBuffer(voice, &buffer, NULL);
            IXAudio2SourceVoice_Start(voice, 0, 0);
            IXAudio2SourceVoice_SetVolume(voice, g_soundVolume, XAUDIO2_COMMIT_NOW);

            while( SUCCEEDED( hr ) && isRunning )
            {
                IXAudio2SourceVoice_GetState(voice, &state );
                isRunning = ( state.BuffersQueued > 0 ) != 0;

                Sleep( 10 );
            }
            
            IXAudio2SourceVoice_FlushSourceBuffers(voice);
            IXAudio2SourceVoice_DestroyVoice(voice);

        }

        MemoryFree(soundMemory);
    }
#endif
}

byte_t DebugAudioGetNextMusicId(byte_t id)
{
    byte_t nextId = id + 1;
    if (nextId > ARRAYSIZE(g_doomMusic))
    {
        nextId = 0;
    }

    return nextId;
}

void DebugAudioPlayMusicByIndex(byte_t id)
{
    if (id > ARRAYSIZE(g_doomMusic))
    {
        return;
    }

    DebugAudioPlayMusic(g_doomMusic[id]);
}

void DebugAudioPlayMusic(const char* name)
{
    void* soundMemory = NULL;
    long soundMemorySize = 0;

    //if (!g_IXAudio2)
    {
      //  DebugOut("Audio is not initialized");
        //return;
    }

    if (WadGetLump(name, &soundMemory, &soundMemorySize))
    {
        DebugAudioPlayMusData(soundMemory, soundMemorySize);
        MemoryFree(soundMemory);
    }
}