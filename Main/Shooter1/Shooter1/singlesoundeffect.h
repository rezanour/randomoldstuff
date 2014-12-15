#pragma once

class SingleSoundEffect
{
public:
    SingleSoundEffect() : _soundEffect(nullptr)
    {}

    SingleSoundEffect(_In_ const std::shared_ptr<SoundEffect>& soundEffect) : _soundEffect(soundEffect)
    {
        assert(soundEffect != nullptr);
    }

    // Allow copies
    SingleSoundEffect(const SingleSoundEffect& other)
        : _soundEffect(other._soundEffect)
    {}

    // Allow moves
    SingleSoundEffect(SingleSoundEffect&& other)
    {
        _soundEffect.swap(other._soundEffect);
    }

    bool IsValid() const { return _soundEffect != nullptr; }
    void Play() {_soundEffect->Play();}

private:
    std::shared_ptr<DirectX::SoundEffect> _soundEffect;
};
