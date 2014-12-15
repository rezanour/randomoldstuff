#include "AudioDeviceOpenAL.h"
#include "AudioClipOpenAL.h"
#include "AudioContextOpenAL.h"
#include "AudioSourceOpenAL.h"
#include <GDKError.h>
#include <Log.h>
#include <al.h>
#include <algorithm>

using namespace GDK;

AudioDeviceOpenAL::AudioDeviceOpenAL() : _context(AudioContextOpenAL::Create()), _volume(0), _listenerPosition(0, 0, 0), _nextLoopingSourceId(0)
{    
    // Initialize the listener position and volume
    SetVolume(1.0f);
    SetListenerPosition(Vector3(0, 0, 0));
}

AudioDeviceOpenAL::~AudioDeviceOpenAL()
{
    _activeSources.clear();
    _loopingSources.clear();
}

void AudioDeviceOpenAL::Tick()
{
    std::vector<std::shared_ptr<AudioSourceOpenAL>> sourcesToDestroy;
    for (auto source : _activeSources)
    {
        if (!source->IsPlaying())
        {
            sourcesToDestroy.push_back(source);
        }
    }

    for (auto source : sourcesToDestroy)
    {
        _activeSources.remove(source);
    }
}

float AudioDeviceOpenAL::GetVolume() const
{
    return _volume;
}

_Use_decl_annotations_
void AudioDeviceOpenAL::SetVolume(float volume)
{
    _volume = volume;
    _context->MakeCurrent();
	CHECK_AL(alListenerf(AL_GAIN, volume));
}

Vector3 AudioDeviceOpenAL::GetListenerPosition() const 
{
    return _listenerPosition;
}

_Use_decl_annotations_
void AudioDeviceOpenAL::SetListenerPosition(const GDK::Vector3& worldPosition)
{
    _listenerPosition = worldPosition;
    _context->MakeCurrent();
    CHECK_AL(alListenerfv(AL_POSITION, &worldPosition.x));
}

_Use_decl_annotations_
std::shared_ptr<AudioClip> AudioDeviceOpenAL::CreateAudioClip(const std::shared_ptr<AudioContent>& audioContent)
{
    return AudioClipOpenAL::Create(_context, audioContent);
}

_Use_decl_annotations_
void AudioDeviceOpenAL::PlayClip(const std::shared_ptr<AudioClip>& clip)
{
	_activeSources.push_back(PlayClipInternal(clip, Vector3(), false, false));
}

_Use_decl_annotations_
void AudioDeviceOpenAL::PlayClip(const std::shared_ptr<AudioClip>& clip, const GDK::Vector3& worldPosition)
{
	_activeSources.push_back(PlayClipInternal(clip, worldPosition, true, false));
}

_Use_decl_annotations_
int32_t AudioDeviceOpenAL::PlayLoopingClip(const std::shared_ptr<AudioClip>& clip)
{
	auto source = PlayClipInternal(clip, Vector3(), false, true);
    _loopingSources[_nextLoopingSourceId++] = source;
    return _nextLoopingSourceId - 1;
}

_Use_decl_annotations_
int32_t AudioDeviceOpenAL::PlayLoopingClip(const std::shared_ptr<AudioClip>& clip, const GDK::Vector3& worldPosition)
{
	auto source = PlayClipInternal(clip, worldPosition, true, true);
    _loopingSources[_nextLoopingSourceId++] = source;
    return _nextLoopingSourceId - 1;
}

_Use_decl_annotations_
void AudioDeviceOpenAL::SetLoopingClipPosition(int32_t id, const GDK::Vector3& worldPosition)
{
    auto source = _loopingSources.find(id);
    _context->MakeCurrent();
    CHECK_TRUE(source != _loopingSources.end());
    source->second->SetPosition(worldPosition);
}

_Use_decl_annotations_
void AudioDeviceOpenAL::StopLoopingClip(int32_t id)
{
    auto itr = _loopingSources.find(id);
    if (itr != _loopingSources.end())
	{
        itr->second->Stop();
        _loopingSources.erase(itr);
	}
}

void AudioDeviceOpenAL::StopAllLoopingClips()
{
    _loopingSources.clear();
}

_Use_decl_annotations_
std::shared_ptr<AudioSourceOpenAL> AudioDeviceOpenAL::PlayClipInternal(const std::shared_ptr<AudioClip>& clip, const GDK::Vector3& worldPosition, bool isPositional, bool isLooping)
{
    // Create and configure a source
    std::shared_ptr<AudioSourceOpenAL> source = AudioSourceOpenAL::Create(_context, isPositional, isLooping);

	if (isPositional)
	{
        source->SetPosition(worldPosition);
	}

    source->BindAudioClip(clip);
    source->Play();

    return source;
}