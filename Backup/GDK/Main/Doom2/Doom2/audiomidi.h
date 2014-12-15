#ifndef _AUDIOMIDI_H_
#define _AUDIOMIDI_H_

BOOL AudioMidiInitialize();
void AudioMidiUninitialize();
BOOL AudioMidiStreamInitialize();
void AudioMidiStreamUninitialize();
byte_t AudioMidiSetVolume(byte_t volume);
void AudioMidiHandleMessage(UINT Msg, WPARAM wParam, LPARAM lParam);

#endif