#pragma once

#include <RuntimeObject.h>
#include <memory>

namespace GDK
{
    class AudioContextXAudio : public RuntimeObject<AudioContextXAudio>
    {
    public:
        static std::shared_ptr<AudioContextXAudio> Create();

        ~AudioContextXAudio();

    private:
        AudioContextXAudio();
    };
}