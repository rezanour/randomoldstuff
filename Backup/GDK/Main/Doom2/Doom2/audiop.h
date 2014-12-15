#ifndef _AUDIOP_H_
#define _AUDIOP_H_

byte_t* AudioLoadSoundBuffer(const char* name);
void AudioFreeSound(byte_t* sound);

#pragma pack(push,1)

typedef struct
{
    uint_t datasize;
    void*  context;
    byte_t data[1];
} sound_t;

#define GetMUSEventTypeFromEvent(e) ((byte_t)((e & 0x7F) >> 4))
#define GetMUSChannelFromEvent(e) ((byte_t)(e & 0x0F))
#define IsLastMUSEvent(e) ((byte_t)(e & 0x80))

// .MUS file format header
typedef struct
{
    char Id[4]; // Should be "MUS" or 0x1A
    ushort_t  ScorLen;          // length in bytes of score at ScoreStart
    ushort_t  ScoreStart;       // file position of score
    ushort_t  PrimaryChannels;  // how many channels are utilized in the song (channel 15 percussion not included)
    ushort_t  SecondaryChannels;
    ushort_t  IntrumentCount;   // instrument count (ushort_t per id following this header)
    ushort_t  Reserved;
} musheader_t;

// .MUS track processed data (used for MIDI data conversion)
typedef struct
{
  ulong_t  current;
  byte_t   vel;
  long     DeltaTime;
  byte_t   LastEvent;
  byte_t*  data;
} mustrack_t;

// Instruments follow after the header up to 'InstrumentCount'
// ushort_t  Instrument1
// ushort_t  Instrument2
// ushort_t  Instrument3, etc.
//
// Instrument numbers 0-127 are standard  MIDI instruments and 135-181 are standard MIDI purcussions (notes 35-81)

// Notes:
// Unlike MIDI, MUS body data contains only one track.  The body is a sequence of sound events and time records.
// A sound event consists of one or more bytes encoded as follows:
//
// 1st byte is the event descriptor
//  --7----6---5---4-----3---2---1---0-
// |Last | event type | channel number |
//  -----------------------------------
//
// Event types
// 0 - release note
// 1 - play note
// 2 - pitch wheel
// 3 - system event (valueless controller)
// 4 - change controller
// 5 - reserved
// 6 - score end
// 7 - reserved

#define MUS_EVENT_RELEASE_NOTE      0
#define MUS_EVENT_PLAY_NOTE         1
#define MUS_EVENT_PITCH_WHEEL       2
#define MUS_EVENT_SYSTEM            3
#define MUS_EVENT_CHANGE_CONTROLLER 4
#define MUS_EVENT_SCORE_END         6

#define MUS_EVENT_RESERVED1         5
#define MUS_EVENT_RESERVED2         7

#pragma pack(pop)

void DebugAudioPlayMusData(void* musMemory, long musMemorySize);
void AudioPlayMusData(void* musMemory, long musMemorySize);

#endif