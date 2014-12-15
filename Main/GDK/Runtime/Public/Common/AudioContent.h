#pragma once

#include "Platform.h"
#include "RuntimeObject.h"

namespace GDK
{
    class AudioContent : public RuntimeObject<AudioContent>
    {
    public:
        static std::shared_ptr<AudioContent> Create(_Inout_ std::istream &stream);
        
        int16_t GetChannelCount() const { return _channels; }
        int32_t GetSampleRate() const { return _samplesPerSecond; }
        int16_t GetBitRate() const { return _bitsPerSample; }
        int32_t GetDataLength() const { return _dataLength; }
        const byte_t* GetData() const { return _data.get(); }

    private:        
        AudioContent(_In_ int16_t channels, _In_ int32_t samplesPerSecond, _In_ int16_t bitsPerSample, _In_ int32_t dataLength, _In_ std::unique_ptr<byte_t[]>& data);

        int16_t _channels;
        int32_t _samplesPerSecond;
        int16_t _bitsPerSample;
        int32_t _dataLength;
        std::unique_ptr<byte_t[]> _data;
    };
}