#pragma once

struct ISound;

class AudioSystem : public BaseObject<AudioSystem>, public IAudioSystem
{
public:
    static std::shared_ptr<IAudioSystem> Create();
    ~AudioSystem();

    //
    // IAudioSystem
    //

    std::shared_ptr<ISound> CreateSound(_In_ uint8_t* data, _In_ uint32_t dataSize) override;
    void PlaySound(_In_ const std::shared_ptr<ISound>& sound) override;
    void Update() override;

private:
    AudioSystem();

private:
    ComPtr<IXAudio2> _device;
    IXAudio2MasteringVoice* _masteringVoice;

};
