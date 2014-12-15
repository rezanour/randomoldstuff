#pragma once

class Game;
class Scene;
class StaticLevelData;

class GamePlay : public TrackedObject<MemoryTag::GamePlay>
{
public:
    ~GamePlay();

    void Update(_In_ float deltaTime, _In_ bool hasFocus);
    void Draw();

    Scene* GetScene() { return _scene.get(); }
    Content& GetContent() { return _content; }

private:
    friend class Game;
    GamePlay();

private:
    Content _content;
    std::unique_ptr<StaticLevelData> _staticLevelData;
    std::unique_ptr<Scene> _scene;
    GameObject* _gameObject;

    //
    // TEST: simple target-based camera
    //
    bool _cameraDirty;
    XMFLOAT3 _cameraPosition;
    XMFLOAT3 _cameraTarget;
    XMFLOAT4 _cameraOrientation;
    XMFLOAT4X4 _view;
    XMFLOAT4X4 _projection;
};
