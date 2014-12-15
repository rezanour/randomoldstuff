#ifndef _AUDIO_H_
#define _AUDIO_H_

BOOL AudioInitialize();
void AudioUninitialize();
byte_t AudioSetMusicVolume(byte_t volume);
byte_t AudioSetSoundVolume(byte_t volume);

void AudioPlaySound(const char* name);
void AudioPlayMusic(const char* name);
void AudioHandleMessage(UINT Msg, WPARAM wParam, LPARAM lParam); // MCI Audio playback leverages the application's winproc for some notifications.

#endif