#ifndef _AUDIO_H_
#define _AUDIO_H_

extern BOOL g_musicEnabled;
extern BOOL g_soundsEnabled;

typedef USHORT HSOUND;
#define INVALID_HSOUND_VALUE (HSOUND)-1

BOOL AudioInitialize();
VOID AudioShutdown();

/* instanced, non-streamed, non-looping */
HSOUND AudioCreateSFX(LPSTR waveFile);

/* non-instanced, streamed, looping */
HSOUND AudioCreateMusic(LPSTR waveFile);

BOOL AudioIsSFX(HSOUND sound);

VOID AudioDestroy(HSOUND sound);
VOID AudioPlay(HSOUND sound, VEC2 position);

#endif // _AUDIO_H_