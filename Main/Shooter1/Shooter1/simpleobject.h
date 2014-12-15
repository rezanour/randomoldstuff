#pragma once

class SimpleObject : public GameObject
{
public:
    void Update(_In_ float deltaTime) override;

private:
    enum class Flag
    {
        None = 0,
        Rotate = 0x0001,
        AudioEmitter = 0x0002,
    };

private:
    friend class GameObject;
    SimpleObject(_In_ GamePlay* gamePlay, _In_ const GameObjectSpawnInfo* info);

private:
    Flag _flags;
    float _rotationPerSecond;
};
