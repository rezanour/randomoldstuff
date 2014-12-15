#pragma once

class Sound : public BaseObject<Sound>, public ISound
{
public:
    static std::shared_ptr<Sound> CreateSound(_In_ const ComPtr<IXAudio2>& device, _In_ uint32_t dataSize, _In_ const void* data);
    ~Sound();

    IXAudio2SourceVoice* GetVoice() const;
    void GetBuffer(_Out_ XAUDIO2_BUFFER* buffer);

private:
    Sound(_In_ const ComPtr<IXAudio2>& device, _In_ uint32_t dataSize, _In_ const void* data);

    uint8_t* FindChunk(_In_ uint8_t* data, _In_ uint32_t dataSize, _In_ uint32_t fourcc, _Out_ uint32_t* chunkSize);

private:
    // Keep a reference to the XAudio2 device so that it isn't cleaned up
    // before our source voice is, which can cause an AV
    ComPtr<IXAudio2> _device;

    std::vector<uint8_t> _data;
    uint8_t* _audioData;
    uint32_t _audioDataSize;
    IXAudio2SourceVoice* _voice;
};
