#include "Precomp.h"
#include <xaudio2.h>
#include "AudioSystem.h"
#include "AudioSystemImpl.h"
#include "Sound.h"
#include "SoundImpl.h"

std::shared_ptr<IAudioSystem> CreateAudioSystem()
{
    return AudioSystem::Create();
}

std::shared_ptr<IAudioSystem> AudioSystem::Create()
{
    return std::shared_ptr<AudioSystem>(new AudioSystem);
}

AudioSystem::AudioSystem()
{
    HRESULT hr = S_OK;
    hr = XAudio2Create(&_device, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    hr = _device->CreateMasteringVoice(&_masteringVoice, 0,0,0,0, NULL, AudioCategory_SoundEffects);
    if (FAILED(hr))
    {
        throw std::exception();
    }
}

AudioSystem::~AudioSystem()
{
    if (_masteringVoice)
    {
        _masteringVoice->DestroyVoice();
        _masteringVoice = nullptr;
    }
}

_Use_decl_annotations_
std::shared_ptr<ISound> AudioSystem::CreateSound(uint8_t* data, uint32_t dataSize)
{
    return Sound::CreateSound(_device, dataSize, data);
}

_Use_decl_annotations_
void AudioSystem::PlaySound(const std::shared_ptr<ISound>& sound)
{
    XAUDIO2_BUFFER buffer = {0};
    IXAudio2SourceVoice* voice = nullptr;
    Sound* snd = static_cast<Sound*>(sound.get());

    BOOL isRunning = FALSE;
    XAUDIO2_VOICE_STATE state = {0};

    snd->GetBuffer(&buffer);
    voice = snd->GetVoice();

    // GetBuffer();
    // GetVoice();

    voice->GetState(&state, 0);
    isRunning = ( state.BuffersQueued > 0 ) != 0;

    // cancel currently running sound and play new one immediately
    if (isRunning)
    {
        voice->Stop(0, 0);
        voice->FlushSourceBuffers();
    }

    voice->SubmitSourceBuffer(&buffer, NULL);
    voice->Start(0, 0);
    voice->SetVolume(1.0f, XAUDIO2_COMMIT_NOW);
}

void AudioSystem::Update()
{
}
