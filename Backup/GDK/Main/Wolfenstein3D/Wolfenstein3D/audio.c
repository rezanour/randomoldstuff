#include "common.h"
#include <MMSystem.h>

/*
    Each instance of a sound effect requires a different HWAVEOUT. This is the only way I've found to get
    proper mixing and instancing of sound. For non-streamed sounds, a copy of the unprepared header is kept 
    in the sound object and copied to each instance. For streaming music, we don't allow instancing, so it 
    can keep the streaminfo and stream around. Instancing would make this impossible to keep straight.
*/

#define DEFAULT_VOLUME_VALUE 0xFFFF

typedef struct
{
    HMMIO       Stream;
    MMIOINFO    StreamInfo;
    DWORD       TotalSize;
    DWORD       UsedSize;
    MMCKINFO    Riff;
    BOOL        Playing;
} STREAMINFO, *PSTREAMINFO;

typedef struct
{
    BOOL            IsMusic;    /* uses stream instead of instancing */
    WAVEFORMATEX    Format;
    union
    {
        PSTREAMINFO Stream;
        LPWAVEHDR   Data;
    };
} SOUND, *PSOUND;

typedef struct
{
    PSOUND      Sound;
    HWAVEOUT    WaveOut;
    LPWAVEHDR   Header;
    DWORD       Volume;
} SOUNDINSTANCE, *PSOUNDINSTANCE;

#define MAX_SOUNDS 1000
static SOUND g_sounds[MAX_SOUNDS] = {0};
static UINT g_nextFree = 0;

#define MAX_SOUND_INSTANCES 1000
static SOUNDINSTANCE g_soundInstances[MAX_SOUND_INSTANCES] = {0};
static UINT g_nextFreeInstance = 0;

static UINT g_streamChunkSize = 64000;

#define MAX_DESTROY_QUEUE   100
static PSOUNDINSTANCE g_destroyQueue[MAX_DESTROY_QUEUE] = {0};

BOOL g_musicEnabled = TRUE;
BOOL g_soundsEnabled = TRUE;

/**************************************************************
    Internal helper method declarations
***************************************************************/

static VOID CALLBACK WaveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

static HSOUND AudioCreate(LPSTR waveFile, BOOL music);

static BOOL CreateSoundFromWave(PSOUND sound, PSTR waveFile, BOOL music);
static VOID DestroySound(PSOUND sound);

static PSOUNDINSTANCE CreateSoundInstance(PSOUND sound);
static VOID DestroySoundInstance(PSOUNDINSTANCE instance);

static VOID DestroyQueuedInstances();
static VOID AddInstanceToDestroyQueue(PSOUNDINSTANCE instance);

/**************************************************************
    Public Audio API
***************************************************************/

BOOL AudioInitialize()
{
    return TRUE;
}

VOID AudioShutdown()
{
    UINT i;

    for (i = 0; i < ARRAYSIZE(g_sounds); ++i)
    {
        DestroySound(&g_sounds[i]);
    }
}

HSOUND AudioCreate(LPSTR waveFile, BOOL music)
{
    HSOUND sound = INVALID_HSOUND_VALUE;

    if (g_nextFree < MAX_SOUNDS && waveFile)
    {
        if (CreateSoundFromWave(&g_sounds[g_nextFree], waveFile, music))
        {
            sound = g_nextFree;
            g_nextFree++;
            for (; g_nextFree < MAX_SOUNDS; ++g_nextFree)
            {
                if (g_sounds[g_nextFree].Data == NULL)
                    break;
            }
        }
    }

    return sound;
}

HSOUND AudioCreateSFX(LPSTR waveFile)
{
    return AudioCreate(waveFile, FALSE);
}

HSOUND AudioCreateMusic(LPSTR waveFile)
{
    return AudioCreate(waveFile, TRUE);
}

BOOL AudioIsSFX(HSOUND sound)
{
    if (sound != INVALID_HSOUND_VALUE &&
        sound < MAX_SOUNDS)
    {
        return !g_sounds[sound].IsMusic;
    }

    return FALSE;
}

VOID AudioDestroy(HSOUND sound)
{
    if (sound != INVALID_HSOUND_VALUE &&
        sound < MAX_SOUNDS)
    {
        assert(g_sounds[sound].Data);
        DestroySound(&g_sounds[sound]);

        if (sound < g_nextFree)
        {
            g_nextFree = sound;
        }
    }
}

VOID AudioPlay(HSOUND sound, VEC2 position)
{
    /* use the opportunity to clean up */
    DestroyQueuedInstances();

    if (sound != INVALID_HSOUND_VALUE &&
        sound < MAX_SOUNDS)
    {
        float dist = Vec2Len(Vec2Add(position, Vec2Neg(g_player.Position)));
        float maxDist = 15;
        float falloff = 1 - max(min(dist / maxDist, 1), 0);

        assert(g_sounds[sound].Data);

        if ((AudioIsSFX(sound) && g_soundsEnabled) ||
            (!AudioIsSFX(sound) && g_musicEnabled))
        {
            PSOUNDINSTANCE instance = CreateSoundInstance(&g_sounds[sound]);
            if (instance && falloff > 0)
            {
                waveOutSetVolume(instance->WaveOut, instance->Volume * falloff);
                waveOutWrite(instance->WaveOut, instance->Header, sizeof(WAVEHDR));
            }
        }
    }
}

/**************************************************************
    Internal helper methods
***************************************************************/

VOID CALLBACK WaveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    USHORT i;

    switch (uMsg)
    {
    case WOM_DONE:
        {
            LPWAVEHDR header = (LPWAVEHDR)dwParam1;
            PSOUNDINSTANCE instance = (PSOUNDINSTANCE)dwInstance;

            assert(instance->Header == header);
            if (instance->Sound->IsMusic)
            {
                /* need to continue the stream */
                DWORD remaining;
                PSTREAMINFO streamInfo = instance->Sound->Stream;

                /* cleanup current chunk */
                waveOutUnprepareHeader(instance->WaveOut, instance->Header, sizeof(WAVEHDR));

                /* is there more to play? */
                remaining = streamInfo->TotalSize - streamInfo->UsedSize;

                if (remaining > 0)
                {
                    MMRESULT mr = 0;
                    DWORD current, size;

                    size = min(g_streamChunkSize, remaining);
                    streamInfo->UsedSize += size;
                    instance->Header->dwBufferLength = size;

                    for (current = 0; current < size && mr == MMSYSERR_NOERROR; ++current)
                    {
                        if (streamInfo->StreamInfo.pchNext == streamInfo->StreamInfo.pchEndRead)
                        {
                            mr = mmioAdvance(streamInfo->Stream, &streamInfo->StreamInfo, MMIO_READ);
                            if (mr == MMSYSERR_NOERROR)
                            {
                                mr = (streamInfo->StreamInfo.pchNext != streamInfo->StreamInfo.pchEndRead ? MMSYSERR_NOERROR : MMSYSERR_ERROR);
                            }
                        }

                        if (mr == MMSYSERR_NOERROR)
                        {
                            instance->Header->lpData[current] = *streamInfo->StreamInfo.pchNext;
                            ++streamInfo->StreamInfo.pchNext;
                        }
                    }

                    /* ensure header is ready, and start playing next chunk */
                    waveOutPrepareHeader(instance->WaveOut, instance->Header, sizeof(WAVEHDR));
                    waveOutSetVolume(instance->WaveOut, instance->Volume);
                    waveOutWrite(instance->WaveOut, instance->Header, sizeof(WAVEHDR));
                }
                else
                {
                    /* TODO: Should music always loop? or should we add a flag for it? */
                    streamInfo->Playing = FALSE;
                    streamInfo->UsedSize = 0;

                    AddInstanceToDestroyQueue(instance);
                }
            }
            else
            {
                /* sound fx instances should get cleaned up */
                AddInstanceToDestroyQueue(instance);
            }
        }
        break;
    }
}

BOOL CreateSoundFromWave(PSOUND sound, PSTR waveFile, BOOL music)
{
    MMRESULT mr = MMSYSERR_ERROR;
    HMMIO hmmio = NULL;

    ZeroMemory(sound, sizeof(SOUND));

    hmmio = mmioOpen(waveFile, NULL, MMIO_ALLOCBUF | MMIO_READ);
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
                    memcpy(&sound->Format, &pcmFormat, sizeof(pcmFormat));
                    sound->Format.cbSize = 0;
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
            if (music)
            {
                /* if this is music, store off the streaming info and stop */
                sound->IsMusic = TRUE;
                sound->Stream = (PSTREAMINFO)malloc(sizeof(STREAMINFO));
                sound->Stream->Playing = FALSE;
                sound->Stream->Stream = hmmio;
                sound->Stream->TotalSize = chunk.cksize;
                sound->Stream->Riff = riff;
                sound->Stream->UsedSize = 0;

                mr = mmioGetInfo(hmmio, &sound->Stream->StreamInfo, 0);
            }
            else
            {
                /* if this is sound fx, then copy the buffer to the sound object so we can tear off instances */
                MMIOINFO info = {0};
                DWORD current;
                DWORD size = chunk.cksize;

                sound->IsMusic = FALSE;
                sound->Data = (LPWAVEHDR)malloc(sizeof(WAVEHDR));
                ZeroMemory(sound->Data, sizeof(WAVEHDR));

                mr = mmioGetInfo(hmmio, &info, 0);

                sound->Data->lpData = (LPSTR)malloc(size);
                sound->Data->dwBufferLength = size;

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
                        sound->Data->lpData[current] = *info.pchNext;
                        ++info.pchNext;
                    }
                }
            }
        }
    }

    if (mr == 0)
    {
        if (!music)
        {
            mmioClose(hmmio, 0);
        }

        return TRUE;
    }
    else
    {
        /* clean up any stank */
        DestroySound(sound);

        if (hmmio)
        {
            mmioClose(hmmio, 0);
        }

        return FALSE;
    }
}

VOID DestroySound(PSOUND sound)
{
    /* track down and kill off any instances of this sound */
    UINT i;
    for (i = 0; i < ARRAYSIZE(g_soundInstances); ++i)
    {
        if (g_soundInstances[i].Sound == sound)
        {
            DestroySoundInstance(&g_soundInstances[i]);
        }
    }

    if (sound->IsMusic)
    {
        if (sound->Stream)
        {
            if (sound->Stream->Stream)
            {
                mmioClose(sound->Stream->Stream, 0);
            }

            free(sound->Stream);
        }
    }
    else
    {
        if (sound->Data)
        {
            if (sound->Data->lpData)
            {
                free(sound->Data->lpData);
            }

            free(sound->Data);
        }
    }

    ZeroMemory(sound, sizeof(SOUND));
}

PSOUNDINSTANCE CreateSoundInstance(PSOUND sound)
{
    UINT nextFree = g_nextFreeInstance;

    if (nextFree < MAX_SOUND_INSTANCES)
    {
        MMRESULT mr = 0;
        PSOUNDINSTANCE instance = &g_soundInstances[nextFree];
        instance->Volume = DEFAULT_VOLUME_VALUE;

        instance->Sound = sound;

        instance->Header = (LPWAVEHDR)malloc(sizeof(WAVEHDR));
        ZeroMemory(instance->Header, sizeof(WAVEHDR));

        if (sound->IsMusic)
        {
            DWORD current;
            PSTREAMINFO stream = sound->Stream;

            instance->Header->lpData = (LPSTR)malloc(g_streamChunkSize);
            instance->Header->dwBufferLength = g_streamChunkSize;

            mr = mmioGetInfo(stream->Stream, &stream->StreamInfo, 0);

            for (current = 0; current < g_streamChunkSize && mr == MMSYSERR_NOERROR; ++current)
            {
                if (stream->StreamInfo.pchNext == stream->StreamInfo.pchEndRead)
                {
                    mr = mmioAdvance(stream->Stream, &stream->StreamInfo, MMIO_READ);
                    if (mr == MMSYSERR_NOERROR)
                    {
                        mr = (stream->StreamInfo.pchNext != stream->StreamInfo.pchEndRead ? MMSYSERR_NOERROR : MMSYSERR_ERROR);
                    }
                }

                if (mr == MMSYSERR_NOERROR)
                {
                    instance->Header->lpData[current] = *stream->StreamInfo.pchNext;
                    ++stream->StreamInfo.pchNext;
                }
            }
        }
        else
        {
            /* sound fx */

            /* first shallow copy the top level header */
            memcpy(instance->Header, sound->Data, sizeof(WAVEHDR));

            /* then alloc and copy data */
            instance->Header->lpData = (LPSTR)malloc(sound->Data->dwBufferLength);
            memcpy(instance->Header->lpData, sound->Data->lpData, sound->Data->dwBufferLength);
        }

        if (mr == 0)
        {
            if (waveOutGetNumDevs() > 0)
            {
                mr = waveOutOpen(&instance->WaveOut, 0, &sound->Format, (DWORD_PTR)WaveOutProc, (DWORD_PTR)instance, CALLBACK_FUNCTION | WAVE_MAPPED);
            }
        }

        if (mr == 0)
        {
            ++g_nextFreeInstance;
            for (; g_nextFreeInstance < MAX_SOUND_INSTANCES; ++g_nextFreeInstance)
            {
                if (!g_soundInstances[g_nextFreeInstance].WaveOut)
                {
                    break;
                }
            }

            waveOutPrepareHeader(instance->WaveOut, instance->Header, sizeof(WAVEHDR));
            return instance;
        }
        else
        {
            free(instance->Header->lpData);
            free(instance->Header);
            ZeroMemory(instance, sizeof(SOUNDINSTANCE));
        }
    }

    return NULL;
}

/* DO NOT CALL FROM CALLBACK THREAD (audio thread)!!!! */
VOID DestroySoundInstance(PSOUNDINSTANCE instance)
{
    UINT index = instance - g_soundInstances;

    if (instance->WaveOut)
    {
        waveOutPause(instance->WaveOut);

        if (instance->Header)
        {
            waveOutUnprepareHeader(instance->WaveOut, instance->Header, sizeof(WAVEHDR));
            free(instance->Header->lpData);
            free(instance->Header);
        }

        waveOutClose(instance->WaveOut);
    }

    ZeroMemory(instance, sizeof(SOUNDINSTANCE));

    if (index < g_nextFreeInstance)
    {
        g_nextFreeInstance = index;
    }
}

VOID DestroyQueuedInstances()
{
    UINT i;
    for (i = 0; i < ARRAYSIZE(g_destroyQueue); ++i)
    {
        if (g_destroyQueue[i])
        {
            DestroySoundInstance(g_destroyQueue[i]);
            g_destroyQueue[i] = NULL;
        }
    }
}

VOID AddInstanceToDestroyQueue(PSOUNDINSTANCE instance)
{
    UINT i;
    for (i = 0; i < ARRAYSIZE(g_destroyQueue); ++i)
    {
        if (!g_destroyQueue[i])
        {
            g_destroyQueue[i] = instance;
        }
    }
}