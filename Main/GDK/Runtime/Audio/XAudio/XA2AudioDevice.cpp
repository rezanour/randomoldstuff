#include "XA2AudioDevice.h"
#include "XA2AudioClip.h"
#include "XA2AudioContext.h"
#include "XA2AudioSource.h"
#include <GDKError.h>
#include <Log.h>
#include <algorithm>

using namespace GDK;

AudioDeviceXAudio::AudioDeviceXAudio() : _context(AudioContextXAudio::Create()), _volume(0), _listenerPosition(0, 0, 0), _nextLoopingSourceId(0)
{    
    // Initialize the listener position and volume
    SetVolume(1.0f);
    SetListenerPosition(Vector3(0, 0, 0));
}

AudioDeviceXAudio::~AudioDeviceXAudio()
{
    _activeSources.clear();
    _loopingSources.clear();
}

void AudioDeviceXAudio::Tick()
{
    std::vector<std::shared_ptr<AudioSourceXAudio>> sourcesToDestroy;
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

float AudioDeviceXAudio::GetVolume() const
{
    return _volume;
}

_Use_decl_annotations_
void AudioDeviceXAudio::SetVolume(float volume)
{
    _volume = volume;
}

Vector3 AudioDeviceXAudio::GetListenerPosition() const 
{
    return _listenerPosition;
}

_Use_decl_annotations_
void AudioDeviceXAudio::SetListenerPosition(const GDK::Vector3& worldPosition)
{
    _listenerPosition = worldPosition;
}

_Use_decl_annotations_
std::shared_ptr<AudioClip> AudioDeviceXAudio::CreateAudioClip(const std::shared_ptr<AudioContent>& audioContent)
{
    return AudioClipXAudio::Create(_context, audioContent);
}

_Use_decl_annotations_
void AudioDeviceXAudio::PlayClip(const std::shared_ptr<AudioClip>& clip)
{
    _activeSources.push_back(PlayClipInternal(clip, Vector3(), false, false));
}

_Use_decl_annotations_
void AudioDeviceXAudio::PlayClip(const std::shared_ptr<AudioClip>& clip, const GDK::Vector3& worldPosition)
{
    _activeSources.push_back(PlayClipInternal(clip, worldPosition, true, false));
}

_Use_decl_annotations_
int32_t AudioDeviceXAudio::PlayLoopingClip(const std::shared_ptr<AudioClip>& clip)
{
    auto source = PlayClipInternal(clip, Vector3(), false, true);
    _loopingSources[_nextLoopingSourceId++] = source;
    return _nextLoopingSourceId - 1;
}

_Use_decl_annotations_
int32_t AudioDeviceXAudio::PlayLoopingClip(const std::shared_ptr<AudioClip>& clip, const GDK::Vector3& worldPosition)
{
    auto source = PlayClipInternal(clip, worldPosition, true, true);
    _loopingSources[_nextLoopingSourceId++] = source;
    return _nextLoopingSourceId - 1;
}

_Use_decl_annotations_
void AudioDeviceXAudio::SetLoopingClipPosition(int32_t id, const GDK::Vector3& worldPosition)
{
    auto source = _loopingSources.find(id);
    CHECK_TRUE(source != _loopingSources.end());
    source->second->SetPosition(worldPosition);
}

_Use_decl_annotations_
void AudioDeviceXAudio::StopLoopingClip(int32_t id)
{
    auto itr = _loopingSources.find(id);
    if (itr != _loopingSources.end())
    {
        itr->second->Stop();
        _loopingSources.erase(itr);
    }
}

void AudioDeviceXAudio::StopAllLoopingClips()
{
    _loopingSources.clear();
}

_Use_decl_annotations_
std::shared_ptr<AudioSourceXAudio> AudioDeviceXAudio::PlayClipInternal(const std::shared_ptr<AudioClip>& clip, const GDK::Vector3& worldPosition, bool isPositional, bool isLooping)
{
    // Create and configure a source
    std::shared_ptr<AudioSourceXAudio> source = AudioSourceXAudio::Create(_context, isPositional, isLooping);

    if (isPositional)
    {
        source->SetPosition(worldPosition);
    }

    source->BindAudioClip(clip);
    source->Play();

    return source;
}