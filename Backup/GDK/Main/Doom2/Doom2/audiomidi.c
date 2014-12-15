#include "common.h"
#include "audiop.h"
#include <MMSystem.h>

HMIDISTRM g_midiStream = NULL;
uint_t g_midiDevice = 0;
byte_t g_midiVolume = 100;
ushort_t g_midiTimeDivision = 89;

long g_midiDataSize = 0;
byte_t* g_midiDataBuffer = NULL;
long g_midiFileDataSize = 0;
byte_t* g_midiFileDataBuffer = NULL;
MIDIHDR g_midiHeader = {0};

#define TEMP_MIDI_FILE "music.midi"

// Preformatted buffers for quick MIDI file and track authoring
// These were stolen from popular MIDI writers and are common filler
// data bits
#define MIDI_TRACK_HEADER_MARKER    "MTrk"
#define PREFORMED_MIDI_HEADER_DATA  "MThd\000\000\000\006\000\001"
#define PREFORMED_MIDI_TRACKDATA_1  "\000\377\003\035"
#define PREFORMED_MIDI_TRACKDATA_2  "\000\377\057\000"
#define PREFORMED_MIDI_TRACKDATA_3  "\000\377\002\026"
#define PREFORMED_MIDI_TRACKDATA_4  "\000\377\131\002\000\000"
#define PREFORMED_MIDI_TRACKDATA_5  "\000\377\121\003\011\243\032"
#define PREFORMED_MIDI_TRACKDATA_6  "\000\377\057\000"

const uint_t MIDI_TRACK_BUFFERSIZE = 65536;
#define MAX_MIDI_TRACK_COUNT 16

#define MIDI_FILE_MARKER        0x6468544D
#define MIDI_TRACK_MARKER       0x6B72544D
void AudioMidiProc(HMIDIOUT hMidi, uint_t uMsg, ulong_t dwInstanceData, ulong_t dwParam1, ulong_t dwParam2);

#define WORDSWAP(w) (((w) >> 8) | \
                    (((w) << 8) & 0xFF00))

#define DWORDSWAP(dw) (((dw) >> 24) | \
                      (((dw) >> 8) & 0x0000FF00) | \
                      (((dw) << 8) & 0x00FF0000) | \
                      (((dw) << 24) & 0xFF000000))

#define MIDIEVENT_CHANNEL(dw)   (dw & 0x0000000F)
#define MIDIEVENT_TYPE(dw)      (dw & 0x000000F0)
#define MIDIEVENT_DATA1(dw)     ((dw & 0x0000FF00) >> 8)
#define MIDIEVENT_VOLUME(dw)    ((dw & 0x007F0000) >> 16)

#pragma pack(push,1)

///////////////////////////////////////////////////////
// Created to test MCI_SETAUDIO against a non video
// audio playing source
typedef struct 
{
      DWORD_PTR dwCallback;
      DWORD     dwItem;
      DWORD     dwValue;
      DWORD     dwOver;
      LPWSTR    lpstrAlgorithm;
      LPWSTR    lpstrQuality;
} MCI_DGV_SETAUDIO_PARMS;

#define MCI_SETAUDIO             0x0873
#define MCI_DGV_SETAUDIO_VOLUME  0x4002
#define MCI_DGV_SETAUDIO_VALUE   0x01000000
#define MCI_DGV_SETAUDIO_ITEM    0x00800000
///////////////////////////////////////////////////////

unsigned long TestBuffer[] = {
 //0, 0, ((unsigned long)MEVT_LONGMSG << 24) | 8, 0x047F7FF0, 0xF77F7F01,
 //0, 0, ((unsigned long)MEVT_TEMPO   << 24) | 0x0007A120,

 0, 0, 0x007F3C90,
48, 0, 0x00003C90,
 0, 0, 0x007F3C90,
48, 0, 0x00003C90,

 0, 0, 0x007F4390,
48, 0, 0x00004390,
 0, 0, 0x007F4390,
48, 0, 0x00004390,

 0, 0, 0x007F4590,
48, 0, 0x00004590,
 0, 0, 0x007F4590,
48, 0, 0x00004590,

 0, 0, 0x007F4390,
86, 0, 0x00004390,

10, 0, 0x007F4190,
48, 0, 0x00004190,
 0, 0, 0x007F4190,
48, 0, 0x00004190,

 0, 0, 0x007F4090,
48, 0, 0x00004090,
 0, 0, 0x007F4090,
48, 0, 0x00004090,

 0, 0, 0x007F3E90,
48, 0, 0x00003E90,
 0, 0, 0x007F3E90,
48, 0, 0x00003E90,

 0, 0, 0x007F3C90,
96, 0, 0x00003C90};

// .MID or .MIDI file format header
typedef struct
{
    ulong_t   Id;
    ulong_t   Size;
    ushort_t  Format;
    ushort_t  TrackCount;
    ushort_t  TimeDivision;
} midiheader_t;

typedef struct
{
    ulong_t Id;
    ulong_t TrackLength;
} miditrack_t;

typedef struct
{
    long    deltaTime;
    byte_t  channel;
    byte_t  runningStatus;
    byte_t  status;
    byte_t  param1;
    byte_t  param2;
    byte_t* tempData;
    long    dataLength;
    byte_t* nextEvent;
    byte_t* eventData;
} midievent_t;

#pragma pack(pop)

BOOL ConvertMUSToMidi(const char* midiFile, void* musMemory, long musMemorySize);
void AudioMidiClose();
void AudioMidiStop();

BOOL AudioMidiInitialize()
{
    DeleteFile(TEMP_MIDI_FILE);
    return TRUE;
}

BOOL AudioMidiStreamInitialize()
{
    MIDIPROPTIMEDIV mptd = {0};
    mptd.cbStruct = sizeof(mptd);
    mptd.dwTimeDiv = g_midiTimeDivision;

    if((midiStreamOpen( &g_midiStream, &g_midiDevice,  1, (DWORD_PTR)AudioMidiProc,  (DWORD_PTR)NULL, CALLBACK_FUNCTION )) != MMSYSERR_NOERROR )
    {
        DebugOut("Failed to create midi streaming device");
        return FALSE;
    }

    if((midiStreamProperty( g_midiStream, (byte_t*)&mptd, MIDIPROP_SET | MIDIPROP_TIMEDIV )) != MMSYSERR_NOERROR )
    {
        DebugOut("Failed to set time division on midi streaming device");
        return FALSE;
    }

    return TRUE;
}

void AudioMidiUninitialize()
{
    AudioMidiStop();
    AudioMidiClose();
    DeleteFile(TEMP_MIDI_FILE);
}

void AudioMidiStreamUninitialize()
{
    if (g_midiStream)
    {
        midiStreamClose(g_midiStream);
        g_midiStream = NULL;
    }

    if (g_midiDataBuffer)
    {
        MemoryFree(g_midiDataBuffer);
        g_midiDataBuffer = NULL;
        g_midiDataSize = 0;
    }
}

byte_t AudioMidiSetVolume(byte_t volume)
{
    MCI_DGV_SETAUDIO_PARMS params;

    volume = g_midiVolume;

    if (g_midiDevice)
    {   
        params.dwCallback = 0;
        params.dwItem = MCI_DGV_SETAUDIO_VOLUME;
        params.dwValue = volume * 10;
        params.dwOver = 0;
        params.lpstrAlgorithm = NULL;
        params.lpstrQuality = NULL;
        
        mciSendCommand(g_midiDevice, MCI_SETAUDIO, (DWORD_PTR)MCI_DGV_SETAUDIO_VALUE|MCI_DGV_SETAUDIO_ITEM, (DWORD_PTR)&params);
    }

    return g_midiVolume;
}

void AudioMidiClose()
{
    if (g_midiDevice)
    {
        mciSendCommand(g_midiDevice, MCI_CLOSE, (DWORD_PTR)0, (DWORD_PTR)0);
        g_midiDevice = 0;
    }
}

void AudioMidiStop()
{
    MCI_GENERIC_PARMS  mciStopParams = {0};
    if (g_midiDevice)
    {
        mciSendCommand(g_midiDevice, MCI_STOP, (DWORD_PTR)0, (DWORD_PTR)0);
    }
}

void AudioMidiPlayFileWithMCI(const char* filePath)
{
    MCIERROR mciError = 0;
    MCI_OPEN_PARMS mciOpenParms = {0};
    MCI_PLAY_PARMS mciPlayParms = {0};
    MCI_STATUS_PARMS mciStatusParms = {0};

    mciOpenParms.lpstrDeviceType = "sequencer";
    mciOpenParms.lpstrElementName = filePath;

    mciPlayParms.dwCallback = (DWORD_PTR)GameGetWindow();

    AudioMidiStop();
    AudioMidiClose();

    if (mciError = mciSendCommand((MCIDEVICEID)0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPVOID) &mciOpenParms))
    {
        DebugOut("Failed to open midi sequencer device, MCI error = %d", mciError);
        return;
    }

    g_midiDevice = mciOpenParms.wDeviceID;

    mciStatusParms.dwItem = MCI_SEQ_STATUS_PORT;
    if (mciError = mciSendCommand(g_midiDevice, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID) &mciStatusParms))
    {
        DebugOut("Failed to query output port type for midi device, MCI error = %d", mciError);
        AudioMidiClose();
        return;
    }

    if (LOWORD(mciStatusParms.dwReturn) != (ushort_t)MIDI_MAPPER)
    {
        DebugOut("Failed to output port type for midi device is not MIDI_MAPPER");
        AudioMidiClose();
        return;
    }

    if (mciError = mciSendCommand(g_midiDevice, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID) &mciPlayParms))
    {
        DebugOut("Failed to play midi, MCI error = %d", mciError);
        AudioMidiClose();
        return;
    }
}

long fgetfilesize(FILE* file)
{
    long fileSize = 0;
    fseek(file, 0L, SEEK_END); 
    fileSize = ftell(file); 
    fseek(file, 0L, SEEK_SET); 
    return fileSize;
}

long DecodeMidiDataVariableLength(byte_t **p)
{
    byte_t c;
    int len = 0;
    do
    {
        c = **p;
        *p = (*p)+1;
        len = (len << 7) + (c & 0x7f);
    } while (c & 0x80);

    return len;
}

void ParseEvent(byte_t* midi, midievent_t* outMidiEvent)
{
    midievent_t midiEvent;

    midiEvent.deltaTime     = 0;
    midiEvent.channel       = 0;
    midiEvent.runningStatus = 0;
    midiEvent.status        = 0;
    midiEvent.param1        = 0;
    midiEvent.param2        = 0;
    midiEvent.tempData      = NULL;
    midiEvent.dataLength    = 0;
    midiEvent.nextEvent     = NULL;
    midiEvent.eventData     = NULL;

    midiEvent.deltaTime = DecodeMidiDataVariableLength(&midi);
    if (*midi < 128)
    {
        midi--;
    }
    else
    {
        midiEvent.status = *midi >> 4;
        midiEvent.runningStatus = midiEvent.status;
        midiEvent.channel = *midi & 0xf;
    }

    if (midiEvent.status == 0xF)
    {
        if (midiEvent.channel == 0xF) // meta event
        {
            midiEvent.param1     = *(midi + 1);
            midiEvent.tempData   = midi + 2;
            midiEvent.dataLength = DecodeMidiDataVariableLength(&midiEvent.tempData);
            midiEvent.eventData  = midi + 3;
            midiEvent.nextEvent  = midi + 3 + midiEvent.dataLength;
        }
        else
        {
            midiEvent.tempData   = (midi + 1);
            midiEvent.dataLength = DecodeMidiDataVariableLength(&midiEvent.tempData);
            midiEvent.nextEvent = midi + 2 + midiEvent.dataLength;
        }
    }
    else if (midiEvent.status == 0xC || midiEvent.status == 0xD)
    {
        midiEvent.param1 = *(midi + 1);
        midiEvent.nextEvent = midi + 2;
    }
    else
    {
        midiEvent.param1 = *(midi + 1);
        midiEvent.param2 = *(midi + 2);
        // change note on to note off if velocity is 0
        if (midiEvent.status == 9 && midiEvent.param2 == 0)
        {
            midiEvent.status = 8;
        }
        midiEvent.nextEvent = midi + 3;
    }

    memcpy(outMidiEvent, &midiEvent, sizeof(midiEvent));

    DebugOut("------- MIDI EVENT DATA -------");
    DebugOut("status         = 0x%lx", midiEvent.status);
    DebugOut("runningStatus  = %d", midiEvent.runningStatus);
    DebugOut("delta time     = %d", midiEvent.deltaTime);
    DebugOut("channel        = %d", midiEvent.channel);
    DebugOut("param1         = %d", midiEvent.param1);
    DebugOut("param2         = %d", midiEvent.param2);
    DebugOut("data length    = %d", midiEvent.dataLength);
    DebugOut("next event     = %d", midiEvent.nextEvent);
    DebugOut("event data     = %d", midiEvent.eventData);
}

void ParseMidiTrackData(byte_t** midiData, ulong_t midiDataLength)
{
    byte_t* midi = *midiData;
    midievent_t midiEvent;

    DebugOut("------- MIDI TRACK DATA -------");
    ParseEvent(midi, &midiEvent);
    while(midiEvent.nextEvent < midi + midiDataLength)
    {
        ParseEvent(midiEvent.nextEvent, &midiEvent);
    }

    *midiData += midiDataLength;
}

void AudioMidiPlayFileWithStream(const char* filePath)
{
    FILE* midiFile = NULL;
    midiheader_t header = {0};
    BOOL success = TRUE;
    ulong_t headerSize = 0;
    ushort_t midiFormat = 0;
    ushort_t midiTrackCount = 0;
    ushort_t midiTimeDivision = 0;
    
    ushort_t trackIndex = 0;
    miditrack_t track = {0};
    ulong_t midiTrackLength = 0;

    MMRESULT mmresult = 0;
    MIDIEVENT* midiEvent = NULL;
    ulong_t numMidiEvents = 0;
    ulong_t midiEventIndex = 0;
    MIDIEVENT tempMidiEvent = {0};
    byte_t* midiEventBuffer = NULL;

    byte_t* midiFileData = NULL;

    if (fopen_s(&midiFile, filePath, "rb") != 0)
    {
        DebugOut("Failed to open MIDI file %s", filePath);
        return;
    }

    // Allocate a buffer and read the entire file into it
    g_midiFileDataSize  =  fgetfilesize(midiFile); //sizeof(TestBuffer);
    g_midiFileDataBuffer = (byte_t*)MemoryAlloc("MIDI File Data", g_midiFileDataSize);
    
    midiFileData = g_midiFileDataBuffer;

    if (fread_s(g_midiFileDataBuffer, g_midiFileDataSize, g_midiFileDataSize, 1, midiFile) > 0)
    {
        /*
        midiEventBuffer = g_midiDataBuffer;

        memcpy(g_midiDataBuffer, TestBuffer, g_midiDataSize);
        numMidiEvents = g_midiDataSize / (sizeof(DWORD) * 3);
        for (midiEventIndex = 0; midiEventIndex < numMidiEvents;midiEventIndex++)
        {
            // Clear temp event
            ZeroMemory(&tempMidiEvent,sizeof(tempMidiEvent));
            memcpy(&tempMidiEvent, midiEventBuffer, 12); // first 12 bytes of MIDIEVENT structure
            midiEventBuffer += 12;

            midiEvent = &tempMidiEvent;

            DebugOut("Midi Event Info (%d of %d)", midiEventIndex,numMidiEvents);
            DebugOut("dwDeltaTime = %d (0x%lx)",midiEvent->dwDeltaTime,midiEvent->dwDeltaTime);
            DebugOut("dwStreamId  = %d",midiEvent->dwStreamID);
            DebugOut("dwEvent     = %d (0x%lx)",midiEvent->dwEvent,midiEvent->dwEvent);
            DebugOut("dwParam[0]  = %d (0x%lx)",midiEvent->dwParms,midiEvent->dwParms);
            midiEvent++;
        }

        DebugOut("Entire MIDI file buffer of %d bytes read in memory", g_midiDataSize);

        g_midiHeader.dwBufferLength = g_midiDataSize;
        g_midiHeader.lpData = (LPSTR)g_midiDataBuffer;
        g_midiHeader.dwBytesRecorded = g_midiHeader.dwBufferLength;

        mmresult = midiOutPrepareHeader( (HMIDIOUT)g_midiStream, &g_midiHeader, sizeof(MIDIHDR));
        mmresult = midiStreamOut(g_midiStream, &g_midiHeader, sizeof(MIDIHDR));
        mmresult = midiStreamRestart(g_midiStream);
        */
    }
    else
    {
        DebugOut("Failed to read %d bytes of entire MIDI file", g_midiFileDataSize);
    }

    // Read MIDI file data and convert into playable midi stream buffers
    memcpy(&header, midiFileData, sizeof(header));
    midiFileData += sizeof(header);

    // endian conversions
    headerSize       = DWORDSWAP(header.Size);
    midiFormat       = WORDSWAP(header.Format);
    midiTrackCount   = WORDSWAP(header.TrackCount);
    midiTimeDivision = WORDSWAP(header.TimeDivision);

    g_midiTimeDivision = midiTimeDivision;

    DebugOut("header.Id           = 0x%lx", header.Id);
    DebugOut("header.Size         = %d", headerSize);
    DebugOut("header.Format       = %d", midiFormat);
    DebugOut("header.TrackCount   = %d", midiTrackCount);
    DebugOut("header.TimeDivision = %d", midiTimeDivision);

    if (header.Id != MIDI_FILE_MARKER)
    {
        DebugOut("Invalid MIDI file detected");
        success = FALSE;
    }

    if (success)
    {
        for (trackIndex = 0; trackIndex < midiTrackCount; trackIndex++)
        {
            memcpy(&track, midiFileData, sizeof(track));
            midiFileData += sizeof(track);

            // endian conversions
            midiTrackLength = DWORDSWAP(track.TrackLength);

            DebugOut("track.Id          = 0x%lx", track.Id);
            DebugOut("track.TrackLength = %d", midiTrackLength);

            // Read track data buffer
            ParseMidiTrackData(&midiFileData, midiTrackLength);

            //midiFileData += midiTrackLength;

            // initialize next track event value
            if (midiTrackLength == 0)
            {
                DebugOut("End of MIDI track encountered");
            }
            else
            {
                // Read next event due data?
            }
        }
    }

    if (midiFile)
    {
        fclose(midiFile);
    }

    if (g_midiFileDataBuffer)
    {
        MemoryFree(g_midiFileDataBuffer);
        g_midiFileDataSize = 0;
    }
}

void DebugAudioPlayMidiFile(const char* filePath, BOOL useMidiStream)
{
    if (useMidiStream)
    {
        AudioMidiPlayFileWithStream(filePath);
    }
    else
    {
        AudioMidiPlayFileWithMCI(filePath);
    }
}

void DebugAudioPlayMusData(void* musMemory, long musMemorySize)
{
    AudioMidiStop();
    AudioMidiClose();

    DeleteFile(TEMP_MIDI_FILE);
    if (ConvertMUSToMidi(TEMP_MIDI_FILE, musMemory, musMemorySize))
    {
        DebugAudioPlayMidiFile(TEMP_MIDI_FILE, FALSE);
    }
}

void AudioPlayMusData(void* musMemory, long musMemorySize)
{
    AudioMidiStop();
    AudioMidiClose();

    DeleteFile(TEMP_MIDI_FILE);
    if (ConvertMUSToMidi(TEMP_MIDI_FILE, musMemory, musMemorySize))
    {
        AudioMidiPlayFileWithMCI(TEMP_MIDI_FILE);
    }
}

void AudioMidiProc(HMIDIOUT hMidi, uint_t uMsg, ulong_t dwInstanceData, ulong_t dwParam1, ulong_t dwParam2)
{
    MIDIHDR* midiHeader = (MIDIHDR*)dwParam1;
    if (midiHeader)
    {
        switch(uMsg)
        {
        case MOM_OPEN:
            DebugOut("MOM_OPEN");
            break;
        case MOM_CLOSE:
            DebugOut("MOM_CLOSE");
            break;
        case MOM_DONE:
            DebugOut("MOM_DONE");
            //midiStreamOut(g_midiStream, &g_midiHeader, sizeof(MIDIHDR));
            break;
        case MOM_POSITIONCB:
            DebugOut("MOM_POSITIONCB");
            break;
        default:
            DebugOut("Unhandled MOM_XXXX message (0x%d) in AudioMidiProc", uMsg);
            break;
        }
    }
}

//
// Conversion of MUS file data into MIDI file data
//
byte_t FirstChannelAvailable( signed char mus_midiChannel[] )
{
    int i = 0;
    signed char old15 = mus_midiChannel[15], max = -1 ;

    mus_midiChannel[15] = -1;
    for( i = 0 ; i < MAX_MIDI_TRACK_COUNT ; i++ )
    {
        if( mus_midiChannel[i] > max ) 
        {
            max = mus_midiChannel[i];
        }
    }

    mus_midiChannel[15] = old15;

    return (max == 8 ? 10 : max + 1);
}

void TWriteByte( byte_t miditrack, byte_t data, mustrack_t track[] )
{
    uint_t pos = 0;

    pos = track[miditrack].current;
    if( pos < MIDI_TRACK_BUFFERSIZE )
    {
        track[miditrack].data[pos] = data;
    }
    else
    {
        DebugOut("Midi Track buffer full!");
        return;
    }

    track[miditrack].current++ ;
}

void TWriteVarLen( int tracknum, uint_t value, mustrack_t track[] )
{
    uint_t buffer ;

    buffer = value & 0x7f;
    while((value >>= 7))
    {
        buffer <<= 8;
        buffer |= 0x80;
        buffer += (value & 0x7f);
    }

    while(1)
    {
        TWriteByte( tracknum, buffer, track );
        if( buffer & 0x80 )
        {
            buffer >>= 8;
        }
        else
        {
            break;
        }
    }
}

size_t fwrite2(const ushort_t *ptr, size_t size, FILE *file)
{
    ulong_t rev = 0;
    uint_t i = 0;

    for( i = 0 ; i < size ; i++ )
    {
        rev = (rev << 8) + (((*ptr) >> (i*8)) & 0xFF);
    }

    return fwrite( &rev, size, 1, file ) ;
}

void WriteMidiFileHeader( ushort_t ntrks, ushort_t division, FILE *file)
{
    fwrite(PREFORMED_MIDI_HEADER_DATA, 10, 1, file);
    fwrite2(&ntrks, 2, file);
    fwrite2(&division, 2, file);
}

void WriteMidiFileTrackHeader( FILE *file )
{
    ushort_t size = 0;

    size = 43;
    fwrite( MIDI_TRACK_HEADER_MARKER, 4, 1, file );
    fwrite2( &size, 4, file );
    fwrite( PREFORMED_MIDI_TRACKDATA_3 , 4, 1, file );
    fwrite( "DOOM II Midi by Coop  ", 22, 1, file);
    fwrite( PREFORMED_MIDI_TRACKDATA_4, 6, 1, file );
    fwrite( PREFORMED_MIDI_TRACKDATA_5, 7, 1, file );
    fwrite( PREFORMED_MIDI_TRACKDATA_6, 4, 1, file );
}

void WriteMidiFileTrack(int tracknum, FILE *file, mustrack_t track[])
{
    ushort_t size = 0;
    size_t quot = 0;
    size_t rem = 0;

    size = (ushort_t)track[tracknum].current + 4 ;
    fwrite( MIDI_TRACK_HEADER_MARKER, 4, 1, file ) ;
    if( !tracknum )
    {
        size += 33;
    }

    fwrite2( &size, 4, file ) ;
    if( !tracknum)
    {
        fwrite( PREFORMED_MIDI_TRACKDATA_1 "DOOM II MUS to MIDI by Coop  ", 33, 1, file );
    }

    quot = (size_t) (track[tracknum].current / 4096);
    rem = (size_t) (track[tracknum].current - quot * 4096);
    fwrite(track[tracknum].data, 4096, quot, file);
    fwrite(((const unsigned char *) track[tracknum].data) + 4096 * quot, rem, 1, file);
    fwrite(PREFORMED_MIDI_TRACKDATA_2, 4, 1, file);
}

void FreeMUSTracks(mustrack_t track[])
{
    int i = 0;

    for( i = 0 ; i < MAX_MIDI_TRACK_COUNT ; i++ )
    {
        if( track[i].data )
        {
            MemoryFree(track[i].data);
            track[i].data = NULL;
        }
    }
}

BOOL ConvertMUSToMidi(const char* midiFile, void* musMemory, long musMemorySize)
{
    musheader_t* header = NULL;
    ushort_t instrumentIndex = 0;
    ushort_t* instruments = NULL;
    byte_t* data = NULL;
    byte_t* enddata = NULL;
    byte_t musevent = 0;
    byte_t museventType = 0;
    byte_t muschannel = 0;
    ulong_t mustime = 0;
    ulong_t musdeltatime = 0;
    ulong_t mustotaltime = 0;
    byte_t timedata = 0;

    ulong_t musActualTotalTime = 0;
    ulong_t musminutes = 0;
    ulong_t musseconds = 0;

    byte_t newEvent = 0;

    byte_t midichannel = 0;
    byte_t miditrack = 0;
    ushort_t miditrackCount = 0;
    ushort_t miditrackCountInFile = 0;
    ushort_t miditrackTimeDivisionInFile = 89;
    int i = 0;

    // This table maps MUS control codes to MIDI control codes
    byte_t g_mus_midiControlTable[15] = {
        0,    // Program change - not a MIDI control change
        0x00, // Bank select: 0 by default
        0x01, // Modulation pot (frequency vibrato depth)
        0x07, // Volume: 0 = silent, ~100 = normal, 127 = loud
        0x0A, // Pan Balance: 0 = left, 64 = center (default), 127 = right
        0x0B, // Expression pot
        0x5B, // Reverb depth
        0x5D, // Chorus depth
        0x40, // Sustain pedal (hold)
        0x43, // Soft pedal
        0x78, // All sounds off
        0x7B, // All notes off
        0x7E, // Mono
        0x7F, // Poly
        0x79  // Reset all controllers
      };

    signed char mus_midiChannel[MAX_MIDI_TRACK_COUNT];
    byte_t midiChannel_track[MAX_MIDI_TRACK_COUNT];
    mustrack_t mustracks[MAX_MIDI_TRACK_COUNT];

    FILE* file_mid = NULL;
    if (fopen_s(&file_mid, midiFile, "wb") != 0)
    {
        DebugOut("Failed to create %s temporary staging midi file", midiFile);
        return FALSE;
    }

    // intialize MUS to MIDI data structures used for MUS to MIDI conversion
    for (muschannel = 0; muschannel < ARRAYSIZE(mus_midiChannel); muschannel++)
    {
        midiChannel_track[muschannel] = -1;
        mus_midiChannel[muschannel] = -1;
        mustracks[muschannel].current   = 0;
        mustracks[muschannel].vel       = 64;
        mustracks[muschannel].DeltaTime = 0;
        mustracks[muschannel].LastEvent = 0;
        mustracks[muschannel].data      = NULL;
    }

    // set the channel variable back to 0 before entering
    muschannel = 0;

    header = (musheader_t*)musMemory;
    instruments = (ushort_t*)((byte_t*)musMemory + sizeof(musheader_t));

    DebugOut("museheader total size       = %d", sizeof(musheader_t));
    DebugOut("museheader total size + instruments total size = %d", sizeof(musheader_t) + (sizeof(ushort_t) * header->IntrumentCount));

    DebugOut("musheader_t.Id              = %c%c%c%c", header->Id[0],header->Id[1],header->Id[2],header->Id[3]);
    DebugOut("musheader_t.IntrumentCount  = %d",header->IntrumentCount);
    DebugOut("musheader_t.PrimaryChannels = %d",header->PrimaryChannels);
    DebugOut("musheader_t.ScoreStart      = %d",header->ScoreStart);
    DebugOut("musheader_t.ScorLen         = %d",header->ScorLen);

    for (instrumentIndex = 0; instrumentIndex < header->IntrumentCount; instrumentIndex++)
    {
        DebugOut("  Instrument Id: %d",instruments[instrumentIndex]);
    }

    data    = (byte_t*)musMemory;
    enddata = data + musMemorySize;
    data += sizeof(musheader_t) + (header->IntrumentCount * sizeof(ushort_t));

    while(data < enddata)
    {
        musevent     = *data;
        museventType = GetMUSEventTypeFromEvent(musevent);
        muschannel   = GetMUSChannelFromEvent(musevent);

        if( mus_midiChannel[muschannel] == -1 )
        {
            midichannel = mus_midiChannel[muschannel] = (muschannel == 15 ? 9 : FirstChannelAvailable( mus_midiChannel)) ;
            miditrack   = midiChannel_track[midichannel] = (byte_t)miditrackCount++ ;
            mustracks[miditrack].data = (byte_t*)MemoryAlloc("Midi Track Data", MIDI_TRACK_BUFFERSIZE);
        }
        else
        {
            midichannel = mus_midiChannel[muschannel];
            miditrack   = midiChannel_track[midichannel];
        }

        TWriteVarLen( miditrack, mustracks[miditrack].DeltaTime, mustracks);
        mustracks[miditrack].DeltaTime = 0;

        // process MUS event types
        switch(museventType)
        {
            case MUS_EVENT_RELEASE_NOTE:
                {
                    //DebugOut("MUS_EVENT_RELEASE_NOTE, channel %d", muschannel);
                    newEvent = 0x90 | midichannel;

                    // write event
                    if(newEvent != mustracks[miditrack].LastEvent)
                    {
                        TWriteByte( miditrack, newEvent, mustracks ) ;
                        mustracks[miditrack].LastEvent = newEvent ;
                    }

                    // write data
                    data++;
                    TWriteByte(miditrack, *data, mustracks);
                    TWriteByte(miditrack, 0, mustracks);
                }
                break;
            case MUS_EVENT_PLAY_NOTE:
                {
                    //DebugOut("MUS_EVENT_PLAY_NOTE, channel %d", muschannel);
                    newEvent = 0x90 | midichannel;

                    // write event
                    if(newEvent != mustracks[miditrack].LastEvent)
                    {
                        TWriteByte( miditrack, newEvent, mustracks ) ;
                        mustracks[miditrack].LastEvent = newEvent ;
                    }
                    
                    // write data
                    data++;
                    TWriteByte( miditrack, (*data) & 0x7F, mustracks);
                    if( (*data) & 0x80 )
                    {
                        data++;
                        mustracks[miditrack].vel = *data;
                    }
                    TWriteByte( miditrack, mustracks[miditrack].vel, mustracks);
                }
                break;
            case MUS_EVENT_PITCH_WHEEL:
                {
                    //DebugOut("MUS_EVENT_PITCH_WHEEL, channel %d", muschannel);
                    newEvent = 0xE0 | midichannel;

                    // write event
                    if(newEvent != mustracks[miditrack].LastEvent)
                    {
                        TWriteByte( miditrack, newEvent, mustracks ) ;
                        mustracks[miditrack].LastEvent = newEvent ;
                    }

                    // write data
                    data++;
                    TWriteByte( miditrack, ((*data) & 1) << 6, mustracks ) ;
                    TWriteByte( miditrack, (*data) >> 1, mustracks ) ;
                }
                break;
            case MUS_EVENT_SYSTEM:
                {
                    //DebugOut("MUS_EVENT_SYSTEM, channel %d", muschannel);
                    newEvent = 0xB0 | midichannel;

                    // write event
                    if(newEvent != mustracks[miditrack].LastEvent)
                    {
                        TWriteByte( miditrack, newEvent, mustracks ) ;
                        mustracks[miditrack].LastEvent = newEvent ;
                    }

                    // write data
                    data++;
                    TWriteByte(miditrack, g_mus_midiControlTable[*data], mustracks);
                    if( *data == 12 )
                    {
                        TWriteByte(miditrack, header->PrimaryChannels + 1, mustracks);
                    }
                    else
                    {
                        TWriteByte(miditrack, 0, mustracks);
                    }
                }
                break;
            case MUS_EVENT_CHANGE_CONTROLLER:
                {
                    //DebugOut("MUS_EVENT_CHANGE_CONTROLLER, channel %d", muschannel);
                    data++;
                    if (*data)
                    {
                        newEvent = 0xB0 | midichannel;
                        // write event
                        if(newEvent != mustracks[miditrack].LastEvent)
                        {
                            TWriteByte(miditrack, newEvent, mustracks);
                            mustracks[miditrack].LastEvent = newEvent;
                        }

                        TWriteByte(miditrack, g_mus_midiControlTable[*data], mustracks);
                    }
                    else
                    {
                        newEvent = 0xC0 | midichannel;
                        // write event
                        if(newEvent != mustracks[miditrack].LastEvent)
                        {
                            TWriteByte(miditrack, newEvent, mustracks);
                            mustracks[miditrack].LastEvent = newEvent;
                        }
                    }

                    // write data
                    data++;
                    TWriteByte(miditrack, *data, mustracks);
                }
                break;
            case MUS_EVENT_SCORE_END:
                //DebugOut("MUS_EVENT_SCORE_END, channel %d", muschannel);
                break;
            case MUS_EVENT_RESERVED1:
                //DebugOut("MUS_EVENT_RESERVED1, channel %d", muschannel);
                break;
            case MUS_EVENT_RESERVED2:
                //DebugOut("MUS_EVENT_RESERVED2, channel %d", muschannel);
                break;
            default:
                //DebugOut("unhandled event %d, channel %d", museventType, muschannel);
                break;
        }

        // If last event is detected, read time data
        if (IsLastMUSEvent(musevent))
        {
            mustime = 0;
            do
            {
                data++;
                timedata = *data;
                mustime = (mustime << 7) + (timedata & 0x7F);
            } while( (timedata & 0x80));

            musdeltatime = mustime;
            mustotaltime += musdeltatime;

            // update track times
            for(i = 0 ; i < (int)miditrackCount; i++ )
            {
                mustracks[i].DeltaTime += musdeltatime;
            }

            musActualTotalTime = (mustotaltime * 89) / (140 * 89) ;
            musminutes = musActualTotalTime / 60 ;
            musseconds = (byte_t) (musActualTotalTime - musminutes * 60) ;

            //DebugOut("MUS last event detected, (total time = %u:%.2u", musminutes, musseconds);
        }

        // Read next event
        data++;
    }

    //DebugOut("MUS data left to process = %d", enddata - data);

    musActualTotalTime = (mustotaltime * 89) / (140 * 89) ;
    musminutes = musActualTotalTime / 60 ;
    musseconds = (byte_t) (musActualTotalTime - musminutes * 60) ;
    DebugOut("MUS total playing time = %u:%.2u", musminutes, musseconds);

    WriteMidiFileHeader(miditrackCount + 1, 89, file_mid);
    WriteMidiFileTrackHeader(file_mid);

    for( i = 0 ; i < (int)miditrackCount ; i++ )
    {
        WriteMidiFileTrack( i, file_mid, mustracks ) ;
    }

    fflush(file_mid);
    fclose(file_mid);

    FreeMUSTracks(mustracks);

    return TRUE;
}

void AudioMidiHandleMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
    case MM_MCINOTIFY:
        {
            switch(wParam)
            {
            case MCI_NOTIFY_SUCCESSFUL:
                // Looping Audio using MCI for this game has a huge assumption.  To use MCI, which is
                // file based, a temp file TEMP_MIDI_FILE is created and passed to the system.  It is
                // assumed that when this MCI notification is received, we want to loop or play the
                // track again.  Since only one TEMP_MIDI_FILE is used, we can just make another
                // AudioMidiPlayFileWithMCI call passing TEMP_MIDI_FILE.  This was the most simple and
                // less hack way to loop audio using MCI playback.
                AudioMidiPlayFileWithMCI(TEMP_MIDI_FILE);
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }
}
