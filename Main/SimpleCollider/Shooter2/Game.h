#pragma once

// TEMP HACK
#include <SimpleCollider\ShapeCast_p.h>

class Game : NonCopyable<Game>
{
public:
    Game(_In_ Renderer* renderer);

    void Tick();

private:
    void Update(float elapsedTime);
    void Draw();

    void HandleMovement(const XMFLOAT3& movement);

private:
    LARGE_INTEGER _frequency;
    LARGE_INTEGER _lastTime;
    ::Renderer* _renderer;

    Transform _playerTransform;
    XMFLOAT3 _playerVelocity;

    Transform _planeTransforms[4];
    XMFLOAT4X4 _view;
    XMFLOAT4X4 _projection;

    XMFLOAT3 _triangleVertices[8];
    uint32_t _triangleIndices[24];
    std::shared_ptr<GeometricPrimitive> _planes[4];
    std::shared_ptr<GeometricPrimitive> _player;
};
