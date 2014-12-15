#pragma once

struct ISound;

struct __declspec(novtable) IAudioSystem
{
    virtual std::shared_ptr<ISound> CreateSound(_In_ uint8_t* data, _In_ uint32_t dataSize) = 0;
    virtual void PlaySound(_In_ const std::shared_ptr<ISound>& sound) = 0;
    virtual void Update() = 0;
};

std::shared_ptr<IAudioSystem> CreateAudioSystem();
