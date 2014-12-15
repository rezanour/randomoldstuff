#include "XA2AudioSource.h"
#include "XA2AudioContext.h"
#include "XA2AudioClip.h"
#include <AudioDevice.h>

using namespace GDK;

_Use_decl_annotations_
std::shared_ptr<AudioSourceXAudio> AudioSourceXAudio::Create(const std::shared_ptr<AudioContextXAudio>& context, bool isPositional, bool isLooping)
{
    return std::shared_ptr<AudioSourceXAudio>(GDKNEW AudioSourceXAudio(context, isPositional, isLooping));
}

_Use_decl_annotations_
AudioSourceXAudio::AudioSourceXAudio(const std::shared_ptr<AudioContextXAudio>& context, bool isPositional, bool isLooping) : _context(context), _handle(0)
{
    UNREFERENCED_PARAMETER(isPositional);
    UNREFERENCED_PARAMETER(isLooping);
}

AudioSourceXAudio::~AudioSourceXAudio()
{

}

void AudioSourceXAudio::Play()
{

}

void AudioSourceXAudio::Stop()
{

}

bool AudioSourceXAudio::IsPlaying() const
{
    return false;
}

_Use_decl_annotations_
void AudioSourceXAudio::SetPosition(const GDK::Vector3& worldPosition)
{
    UNREFERENCED_PARAMETER(worldPosition);
}

_Use_decl_annotations_
void AudioSourceXAudio::SetVelocity(const GDK::Vector3& velocity)
{
    UNREFERENCED_PARAMETER(velocity);
}

_Use_decl_annotations_
void AudioSourceXAudio::BindAudioClip(const std::shared_ptr<AudioClip>& clip)
{
    UNREFERENCED_PARAMETER(clip);
}