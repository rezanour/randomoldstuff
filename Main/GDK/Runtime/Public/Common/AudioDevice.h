#pragma once

#include "Platform.h"
#include "RuntimeObject.h"
#include "Vector3.h"
#include <vector>

namespace GDK
{
    class AudioContent;
    class AudioClip;

    class AudioDevice : public RuntimeObject<AudioDevice>
    {
    public:
        enum class Type
        {
            OpenAL,
            XAudio
        };

        static std::shared_ptr<AudioDevice> Create(_In_ AudioDevice::Type type);

        virtual void Tick() = 0;
        virtual float GetVolume() const = 0;
        virtual void SetVolume(_In_ float volume) = 0;
        virtual GDK::Vector3 GetListenerPosition() const = 0;
        virtual void SetListenerPosition(_In_ const GDK::Vector3& worldPosition) = 0;
        virtual std::shared_ptr<AudioClip> CreateAudioClip(_In_ const std::shared_ptr<AudioContent>& audioContent) = 0;
        virtual void PlayClip(_In_ const std::shared_ptr<AudioClip>& clip) = 0;
        virtual void PlayClip(_In_ const std::shared_ptr<AudioClip>& clip, _In_ const GDK::Vector3& worldPosition) = 0;
        virtual int32_t PlayLoopingClip(_In_ const std::shared_ptr<AudioClip>& clip) = 0;
        virtual int32_t PlayLoopingClip(_In_ const std::shared_ptr<AudioClip>& clip, _In_ const GDK::Vector3& worldPosition) = 0;
        virtual void SetLoopingClipPosition(_In_ int32_t id, _In_ const GDK::Vector3& worldPosition) = 0;
        virtual void StopLoopingClip(_In_ int32_t id) = 0;
        virtual void StopAllLoopingClips() = 0;

    protected:
        AudioDevice(){}
    };
}
