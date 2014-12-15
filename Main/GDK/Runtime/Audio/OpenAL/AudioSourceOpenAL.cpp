#include "AudioSourceOpenAL.h"
#include "AudioContextOpenAL.h"
#include "AudioClipOpenAL.h"
#include <AudioDevice.h>

using namespace GDK;

_Use_decl_annotations_
std::shared_ptr<AudioSourceOpenAL> AudioSourceOpenAL::Create(const std::shared_ptr<AudioContextOpenAL>& context, bool isPositional, bool isLooping)
{
    return std::shared_ptr<AudioSourceOpenAL>(GDKNEW AudioSourceOpenAL(context, isPositional, isLooping));
}

_Use_decl_annotations_
AudioSourceOpenAL::AudioSourceOpenAL(const std::shared_ptr<AudioContextOpenAL>& context, bool isPositional, bool isLooping) : _context(context), _handle(0)
{
    _context->MakeCurrent();
    CHECK_AL(alGenSources(1, &_handle));
    CHECK_AL(alSourcef(_handle, AL_PITCH, 1));
    CHECK_AL(alSourcef(_handle, AL_GAIN, 1));
    CHECK_AL(alSourcei(_handle, AL_SOURCE_RELATIVE, isPositional ? AL_TRUE : AL_FALSE));
    CHECK_AL(alSourcei(_handle, AL_LOOPING, isLooping ? AL_TRUE : AL_FALSE));
}

AudioSourceOpenAL::~AudioSourceOpenAL()
{
    if (_handle != 0)
    {
        _context->MakeCurrent();
        CHECK_AL(alSourceStop(_handle));
        CHECK_AL(alDeleteSources(1, &_handle));
        _handle = 0;
    }
}

void AudioSourceOpenAL::Play()
{
    CHECK_TRUE(_handle != 0);
    
    _context->MakeCurrent();
    CHECK_AL(alSourcePlay(_handle));
}

void AudioSourceOpenAL::Stop()
{
    CHECK_TRUE(_handle != 0);
    
    _context->MakeCurrent();
    CHECK_AL(alSourceStop(_handle));
}

bool AudioSourceOpenAL::IsPlaying() const
{
    CHECK_TRUE(_handle != 0);

    ALint state = 0;
    _context->MakeCurrent();
    CHECK_AL(alGetSourcei(_handle, AL_SOURCE_STATE, &state));
    return state == AL_PLAYING;
}

_Use_decl_annotations_
void AudioSourceOpenAL::SetPosition(const GDK::Vector3& worldPosition)
{
    CHECK_TRUE(_handle != 0);

    _position = worldPosition;
    _context->MakeCurrent();
    CHECK_AL(alSource3f(_handle, AL_POSITION, _position.x, _position.y, _position.z));
}

_Use_decl_annotations_
void AudioSourceOpenAL::SetVelocity(const GDK::Vector3& velocity)
{
    CHECK_TRUE(_handle != 0);

    _velocity = velocity;
    _context->MakeCurrent();
    CHECK_AL(alSource3f(_handle, AL_VELOCITY, _velocity.x, _velocity.y, _velocity.z));
}

_Use_decl_annotations_
void AudioSourceOpenAL::BindAudioClip(const std::shared_ptr<AudioClip>& clip)
{
    CHECK_TRUE(_handle != 0);

    _activeClip = clip;
    
    AudioClipOpenAL *alClip = static_cast<AudioClipOpenAL*>(clip.get());
    ALuint bufferHandle = alClip->GetBufferHandle();

    // Queue the buffer
    _context->MakeCurrent();
    CHECK_AL(alSourceQueueBuffers(_handle, 1, &bufferHandle));
}