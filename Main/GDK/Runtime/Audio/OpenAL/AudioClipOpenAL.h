#pragma once

#include <AudioClip.h>
#include "GDKAL.h"

namespace GDK
{
    class AudioContent;
    class AudioDevice;
    class AudioContextOpenAL;

    class AudioClipOpenAL : public AudioClip
    {
    public:
        static std::shared_ptr<AudioClipOpenAL> Create(_In_ const std::shared_ptr<AudioContextOpenAL>& context, _In_ const std::shared_ptr<AudioContent>& content);

        ALuint GetBufferHandle() const { return _bufferHandle; }

        ~AudioClipOpenAL();

    private:
        AudioClipOpenAL(_In_ const std::shared_ptr<AudioContextOpenAL>& context, _In_ const std::shared_ptr<AudioContent>& content);

        std::shared_ptr<AudioContextOpenAL> _context;
        ALuint _bufferHandle;
    };
}