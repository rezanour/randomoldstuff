#define WIN32_LEAN_AND_MEAN
#define VCEXTRALEAN
#include <Windows.h>

#include <stdio.h>
#include <MMSystem.h>

HRESULT LoadWaveFile(PWSTR filename, WAVEHDR* waveHeader, WAVEFORMATEX* format);
void DestroyWave(WAVEHDR* waveHeader);

#ifdef OPENAL

#include <al.h>
#include <alc.h>

ALCdevice* g_device = nullptr;
ALCcontext* g_context = nullptr;
ALuint g_buffer = 0;
ALuint g_source = 0;

#elif XAUDIO2

#include <XAudio2.h>

IXAudio2* g_device = nullptr;
IXAudio2MasteringVoice* g_master = nullptr;
IXAudio2SourceVoice* g_voice = nullptr;

#elif WAVE_OUT

HWAVEOUT g_waveOut = nullptr;
LPWAVEHDR g_waveData = nullptr;

#endif

HRESULT InitializeAudio();
HRESULT SimpleTest(WAVEHDR* waveData, WAVEFORMATEX* format);
HRESULT StreamingTest(WAVEHDR* waveData, WAVEFORMATEX* format);
void Shutdown();

int __cdecl wmain(_In_ int argc, _In_ wchar_t** argv)
{
    HRESULT hr = S_OK;
    WAVEHDR waveData = {0};
    WAVEFORMATEX format = {0};

    hr = LoadWaveFile(L"PickupChaingun.wav", &waveData, &format);

    if (SUCCEEDED(hr))
    {
        //Sleep(5000);
        hr = InitializeAudio();
    }

    if (SUCCEEDED(hr))
    {
        //Sleep(5000);
        hr = SimpleTest(&waveData, &format);
    }

    if (SUCCEEDED(hr))
    {
        Sleep(5000);
    }

    DestroyWave(&waveData);

    Shutdown();

    if (FAILED(hr))
    {
        wprintf(L"Error occured!\n");
    }
}

#ifdef OPENAL

HRESULT InitializeAudio()
{
    ALenum result = AL_NO_ERROR;

    g_device = alcOpenDevice(nullptr);

    result = alGetError();
    if (g_device && result == AL_NO_ERROR)
    {
        g_context = alcCreateContext(g_device, nullptr);

        result = alGetError();
        if (g_context && result == AL_NO_ERROR)
        {
            alcMakeContextCurrent(g_context);

            result = alGetError();
            if (result == AL_NO_ERROR)
            {
                alGenBuffers(1, &g_buffer);

                result = alGetError();
                if (result == AL_NO_ERROR)
                {
                    alGenSources(1, &g_source);

                    result = alGetError();
                    if (result == AL_NO_ERROR)
                    {
                        return S_OK;
                    }
                }
            }
        }
    }

    wprintf(L"AL error %d\n", result);

    return E_FAIL;
}

HRESULT SimpleTest(WAVEHDR* waveData, WAVEFORMATEX* format)
{
    ALenum result = AL_NO_ERROR;
    ALenum bufferFormat;

    if (format->nChannels == 1)
    {
        bufferFormat = format->wBitsPerSample == 8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    }
    else
    {
        bufferFormat = format->wBitsPerSample == 8 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    }

    alBufferData(g_buffer, bufferFormat, waveData->lpData, waveData->dwBufferLength, format->nSamplesPerSec);

    result = alGetError();
    if (result == AL_NO_ERROR)
    {
        alSourcei(g_source, AL_BUFFER, g_buffer);

        result = alGetError();
        if (result == AL_NO_ERROR)
        {
            alSourcePlay(g_source);

            result = alGetError();
            if (result == AL_NO_ERROR)
            {
                return S_OK;
            }
        }
    }

    wprintf(L"AL error %d\n", result);

    return E_FAIL;
}

HRESULT StreamingTest(WAVEHDR* waveData, WAVEFORMATEX* format)
{
    return S_OK;
}

void Shutdown()
{
    if (g_context)
    {
        alDeleteSources(1, &g_source);
        alDeleteBuffers(1, &g_buffer);
        alcDestroyContext(g_context);
        g_context = nullptr;
    }
    if (g_device)
    {
        alcCloseDevice(g_device);
        g_device = nullptr;
    }
}

#elif XAUDIO2

HRESULT InitializeAudio()
{
    HRESULT hr = CoInitialize(nullptr);
    if (SUCCEEDED(hr))
    {
        hr = XAudio2Create(&g_device);
        if (SUCCEEDED(hr))
        {
            hr = g_device->CreateMasteringVoice(&g_master);
        }
    }

    return hr;
}

HRESULT SimpleTest(WAVEHDR* waveData, WAVEFORMATEX* format)
{
    XAUDIO2_BUFFER buffer = {0};
    buffer.AudioBytes = waveData->dwBufferLength;
    buffer.pAudioData = (PBYTE)waveData->lpData;
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    HRESULT hr = g_device->CreateSourceVoice(&g_voice, format);
    if (SUCCEEDED(hr))
    {
        hr = g_voice->SubmitSourceBuffer(&buffer);
        if (SUCCEEDED(hr))
        {
            hr = g_voice->Start();
        }
    }

    return S_OK;
}

HRESULT StreamingTest(WAVEHDR* waveData, WAVEFORMATEX* format)
{
    return S_OK;
}

void Shutdown()
{
    if (g_voice)
    {
        g_voice->DestroyVoice();
        g_voice = nullptr;
    }

    if (g_master)
    {
        g_master->DestroyVoice();
        g_master = nullptr;
    }

    if (g_device)
    {
        g_device->Release();
        g_device = nullptr;
    }

    CoUninitialize();
}

#elif WAVE_OUT

HRESULT InitializeAudio()
{
    return S_OK;
}

HRESULT SimpleTest(WAVEHDR* waveData, WAVEFORMATEX* format)
{
    g_waveData = waveData;

    MMRESULT mr = waveOutOpen(&g_waveOut, WAVE_MAPPER, format, 0, 0, CALLBACK_NULL);
    if (mr == 0)
    {
        mr = waveOutPrepareHeader(g_waveOut, g_waveData, sizeof(WAVEHDR));

        if (mr == 0)
        {
            waveOutWrite(g_waveOut, g_waveData, sizeof(WAVEHDR));
        }
    }

    return mr == 0 ? S_OK : E_FAIL;
}

HRESULT StreamingTest(WAVEHDR* waveData, WAVEFORMATEX* format)
{
    return S_OK;
}

void Shutdown()
{
    if (g_waveOut)
    {
        waveOutPause(g_waveOut);
        waveOutUnprepareHeader(g_waveOut, g_waveData, sizeof(WAVEHDR));
        waveOutClose(g_waveOut);
    }
}

#endif

HRESULT LoadWaveFile(PWSTR filename, WAVEHDR* waveHeader, WAVEFORMATEX* format)
{
    MMRESULT mr = MMSYSERR_ERROR;
    HMMIO hmmio = NULL;

    ZeroMemory(waveHeader, sizeof(*waveHeader));

    hmmio = mmioOpen(filename, NULL, MMIO_ALLOCBUF | MMIO_READ);
    if (hmmio != NULL)
    {
        MMCKINFO riff = {0};
        MMCKINFO chunk = {0};
        PCMWAVEFORMAT pcmFormat = {0};

        mr = mmioDescend(hmmio, &riff, NULL, 0);
        if (mr == 0)
        {
            /* ensure it's a proper wave file */
            if (riff.ckid != FOURCC_RIFF || riff.fccType != mmioFOURCC('W', 'A', 'V', 'E'))
            {
                mr = MMSYSERR_ERROR;
            }
        }

        if (mr == 0)
        {
            /* find the format info */
            chunk.ckid = mmioFOURCC( 'f', 'm', 't', ' ' );
            if (mmioDescend(hmmio, &chunk, &riff, MMIO_FINDCHUNK) == 0)
            {
                if (mmioRead(hmmio, (HPSTR)&pcmFormat, sizeof(PCMWAVEFORMAT)) == sizeof(PCMWAVEFORMAT) &&
                    pcmFormat.wf.wFormatTag == WAVE_FORMAT_PCM)
                {
                    memcpy(format, &pcmFormat, sizeof(*format));
                    format->cbSize = 0;
                }
                else
                {
                    mr = MMSYSERR_ERROR;
                }
            }
            else
            {
                mr = MMSYSERR_ERROR;
            }
        }

        if (mr == 0)
        {
            /* find the data chunk */
            if (mmioSeek(hmmio, riff.dwDataOffset + sizeof(FOURCC), SEEK_SET) != -1)
            {
                chunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
                mr = mmioDescend(hmmio, &chunk, &riff, MMIO_FINDCHUNK);
            }
            else
            {
                mr = MMSYSERR_ERROR;
            }
        }

        if (mr == 0)
        {
            MMIOINFO info = {0};
            DWORD current;
            DWORD size = chunk.cksize;

            mr = mmioGetInfo(hmmio, &info, 0);

            waveHeader->lpData = new CHAR[size];
            waveHeader->dwBufferLength = size;

            for (current = 0; current < size && mr == MMSYSERR_NOERROR; ++current)
            {
                if (info.pchNext == info.pchEndRead)
                {
                    mr = mmioAdvance(hmmio, &info, MMIO_READ);
                    if (mr == MMSYSERR_NOERROR)
                    {
                        mr = (info.pchNext != info.pchEndRead ? MMSYSERR_NOERROR : MMSYSERR_ERROR);
                    }
                }

                if (mr == MMSYSERR_NOERROR)
                {
                    waveHeader->lpData[current] = *info.pchNext;
                    ++info.pchNext;
                }
            }
        }

        mmioClose(hmmio, 0);
    }

    if (mr != 0)
    {
        DestroyWave(waveHeader);
    }

    return mr == 0 ? S_OK : E_FAIL;
}

void DestroyWave(WAVEHDR* waveHeader)
{
    delete [] waveHeader->lpData;
    ZeroMemory(waveHeader, sizeof(*waveHeader));
}
