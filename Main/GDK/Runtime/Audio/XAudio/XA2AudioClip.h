#pragma once

#include <AudioClip.h>

namespace GDK
{
    class AudioContent;
    class AudioDevice;
    class AudioContextXAudio;

    class AudioClipXAudio : public AudioClip
    {
    public:
        static std::shared_ptr<AudioClipXAudio> Create(_In_ const std::shared_ptr<AudioContextXAudio>& context, _In_ const std::shared_ptr<AudioContent>& content);
        ~AudioClipXAudio();

    private:
        AudioClipXAudio(_In_ const std::shared_ptr<AudioContextXAudio>& context, _In_ const std::shared_ptr<AudioContent>& content);
        std::shared_ptr<AudioContextXAudio> _context;
    };
}