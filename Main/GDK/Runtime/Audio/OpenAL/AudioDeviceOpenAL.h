#pragma once

#include <AudioDevice.h>
#include "GDKAL.h"
#include <list>
#include <map>

namespace GDK
{
    class AudioClip;
    class AudioContent;
    class AudioSourceOpenAL;
    class AudioContextOpenAL;

    class AudioDeviceOpenAL : public AudioDevice
    {
    public:
        AudioDeviceOpenAL();
        ~AudioDeviceOpenAL();
        
        virtual void Tick() override;
        virtual float GetVolume() const override;
        virtual void SetVolume(_In_ float volume) override;
        virtual GDK::Vector3 GetListenerPosition() const override;
        virtual void SetListenerPosition(_In_ const GDK::Vector3& worldPosition) override;
        virtual std::shared_ptr<AudioClip> CreateAudioClip(_In_ const std::shared_ptr<AudioContent>& audioContent) override;
        virtual void PlayClip(_In_ const std::shared_ptr<AudioClip>& clip) override;
        virtual void PlayClip(_In_ const std::shared_ptr<AudioClip>& clip, _In_ const GDK::Vector3& worldPosition) override;
        virtual int32_t PlayLoopingClip(_In_ const std::shared_ptr<AudioClip>& clip) override;
        virtual int32_t PlayLoopingClip(_In_ const std::shared_ptr<AudioClip>& clip, _In_ const GDK::Vector3& worldPosition) override;
		virtual void SetLoopingClipPosition(_In_ int32_t id, _In_ const GDK::Vector3& worldPosition) override;
        virtual void StopLoopingClip(_In_ int32_t id) override;
        virtual void StopAllLoopingClips() override;

    private:
		std::shared_ptr<AudioSourceOpenAL> PlayClipInternal(
            _In_ const std::shared_ptr<AudioClip>& clip, 
            _In_ const GDK::Vector3& worldPosition, 
            _In_ bool isPositional, 
            _In_ bool isLooping);

        std::shared_ptr<AudioContextOpenAL> _context;

        std::list<std::shared_ptr<AudioSourceOpenAL>> _activeSources;
		std::shared_ptr<AudioSourceOpenAL> _musicSource;
		std::map<int32_t, std::shared_ptr<AudioSourceOpenAL>> _loopingSources;
		int32_t _nextLoopingSourceId;

        float _volume;
        GDK::Vector3 _listenerPosition;
    };
}