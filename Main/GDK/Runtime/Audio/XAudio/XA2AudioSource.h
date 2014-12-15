#pragma once

#include <RuntimeObject.h>
#include <GDKMath.h>

namespace GDK
{
    class AudioContextXAudio;
    class AudioClip;

    class AudioSourceXAudio : public RuntimeObject<AudioSourceXAudio>
    {
    public:
        static std::shared_ptr<AudioSourceXAudio> Create(_In_ const std::shared_ptr<AudioContextXAudio>& context, _In_ bool isPositional, _In_ bool isLooping);

        ~AudioSourceXAudio();

        void Play();
        void Stop();
        bool IsPlaying() const;
        
        GDK::Vector3 GetPosition() const { return _position; }
        GDK::Vector3 GetVelocity() const { return _velocity; }
        size_t GetHandle() const { return _handle; }

        void SetPosition(_In_ const GDK::Vector3& worldPosition);
        void SetVelocity(_In_ const GDK::Vector3& velocity);
        void BindAudioClip(_In_ const std::shared_ptr<AudioClip>& clip);

    protected:
        AudioSourceXAudio(_In_ const std::shared_ptr<AudioContextXAudio>& context, _In_ bool isPositional, _In_ bool isLooping);

    private:
        std::shared_ptr<AudioContextXAudio> _context;
        size_t _handle;
        GDK::Vector3 _position;
        GDK::Vector3 _velocity;
        std::shared_ptr<AudioClip> _activeClip;
    };
}