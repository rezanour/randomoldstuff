#pragma once

class GameScreen;

class GameObject
{
public:
    //
    // Returns shared pointer. GameObjects are created & destroyed often, and we need to track
    // how many references are outstanding for each, so we know when they can be safely deleted.
    // Care should be taken to avoid handing shared references out of GameScreen scope (they should
    // never outlive the GameScreen).
    //
    static std::shared_ptr<GameObject> Create(_In_ GameScreen* screen);

    //
    // Basic transform
    //
    const XMFLOAT3& GetPosition() const     { return _position; }
    const XMFLOAT4& GetOrientation() const  { return _orientation; }
    const XMFLOAT3& GetScale() const        { return _scale; }
    const XMFLOAT4X4& GetWorld() const;

    void SetPosition(_In_ const XMFLOAT3& value)    { _position = value; _worldIsDirty = true; }
    void SetOrientation(_In_ const XMFLOAT4& value) { _orientation = value; _worldIsDirty = true; }
    void SetScale(_In_ const XMFLOAT3& value)       { _scale = value; _worldIsDirty = true; }

    //
    // Flags
    //
    bool IsDead() const         { return _isDead; }
    bool IsVisible() const      { return _isVisible; }
    bool IsSolid() const        { return _isSolid; }

    void SetDead(_In_ bool value)       { _isDead = value; }
    void SetVisible(_In_ bool value)    { _isVisible = value; }
    void SetSolid(_In_ bool value)      { _isSolid = value; }

private:
    GameObject(_In_ GameScreen* screen);

private:
    GameScreen* _screen;

    XMFLOAT3 _position;
    XMFLOAT4 _orientation;
    XMFLOAT3 _scale;
    mutable XMFLOAT4X4 _cachedWorld;

    //
    // Flags
    //
    bool _isDead        : 1;
    bool _isVisible     : 1;
    bool _isSolid       : 1;
    mutable bool _worldIsDirty  : 1;
};
