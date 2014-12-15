#pragma once

#include <RuntimeObject.h>
#include "GDKAL.h"
#include <GDKMath.h>

namespace GDK
{
    class AudioContextOpenAL;
    class AudioClip;

    class AudioSourceOpenAL : public RuntimeObject<AudioSourceOpenAL>
    {
    public:
        static std::shared_ptr<AudioSourceOpenAL> Create(_In_ const std::shared_ptr<AudioContextOpenAL>& context, _In_ bool isPositional, _In_ bool isLooping);

        ~AudioSourceOpenAL();

        void Play();
        void Stop();
        bool IsPlaying() const;
        
        GDK::Vector3 GetPosition() const { return _position; }
        GDK::Vector3 GetVelocity() const { return _velocity; }
        ALuint GetHandle() const { return _handle; }

        void SetPosition(_In_ const GDK::Vector3& worldPosition);
        void SetVelocity(_In_ const GDK::Vector3& velocity);
        void BindAudioClip(_In_ const std::shared_ptr<AudioClip>& clip);

    protected:
        AudioSourceOpenAL(_In_ const std::shared_ptr<AudioContextOpenAL>& context, _In_ bool isPositional, _In_ bool isLooping);

    private:
        std::shared_ptr<AudioContextOpenAL> _context;
        ALuint _handle;
        GDK::Vector3 _position;
        GDK::Vector3 _velocity;
        std::shared_ptr<AudioClip> _activeClip;
    };
}