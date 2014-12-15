#include "Precomp.h"
#include <xaudio2.h>
#include "Sound.h"
#include "SoundImpl.h"

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

#pragma pack(push,1)

typedef struct
{
    uint32_t chunkType;
    uint32_t chunkSize;
} wav_entry_t;

#pragma pack(pop)

Sound::~Sound()
{
    if (_voice)
    {
        _voice->DestroyVoice();
        _voice = nullptr;
    }
}

_Use_decl_annotations_
std::shared_ptr<Sound> Sound::CreateSound(const ComPtr<IXAudio2>& device, uint32_t audioDataSize, const void* audioData)
{
    return std::shared_ptr<Sound>(new Sound(device, audioDataSize, audioData));
}

_Use_decl_annotations_
Sound::Sound(const ComPtr<IXAudio2>& device, uint32_t audioDataSize, const void* audioData) :
    _device(device)
{
    HRESULT hr = S_OK;
    WAVEFORMATEXTENSIBLE* wfx = nullptr;
    uint32_t wfxSize = 0;

    _data.resize(audioDataSize);
    memcpy(_data.data(), audioData, audioDataSize);

    wfx = (WAVEFORMATEXTENSIBLE*)FindChunk((uint8_t*)_data.data(), audioDataSize, fourccFMT, &wfxSize);
    _audioData = FindChunk((uint8_t*)_data.data(), audioDataSize, fourccDATA, &_audioDataSize);
    hr = device->CreateSourceVoice(&_voice, &wfx->Format, 0, 2.0f,0,0,NULL);
    if (FAILED(hr))
    {
        throw std::exception();
    }
}

IXAudio2SourceVoice* Sound::GetVoice() const
{
    return _voice;
}

_Use_decl_annotations_
void Sound::GetBuffer(XAUDIO2_BUFFER* buffer)
{
    ZeroMemory(buffer, sizeof(XAUDIO2_BUFFER));
    buffer->Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
    buffer->AudioBytes = _audioDataSize;
    buffer->pAudioData = _audioData;
}

_Use_decl_annotations_
uint8_t* Sound::FindChunk(uint8_t* data, uint32_t dataSize, uint32_t fourcc, uint32_t* chunkSize)
{
    *chunkSize = 0;
    uint32_t chunkDataSize;
    uint32_t fileType;
    uint8_t* curr = data;
    uint8_t* end = curr + dataSize;

    while(curr <= end)
    {
        wav_entry_t* entry = (wav_entry_t*)curr;
        curr+= sizeof(wav_entry_t);

        switch(entry->chunkType)
        {
        case fourccRIFF:
            chunkDataSize = entry->chunkSize;
            fileType = *((uint32_t*)curr);
            curr += sizeof(uint32_t);
            break;
        default:
            curr += entry->chunkSize;
            break;
        }

        if (entry->chunkType == fourcc)
        {
            *chunkSize = entry->chunkSize;
            return curr - entry->chunkSize;
        }
    }

    return nullptr;
}
