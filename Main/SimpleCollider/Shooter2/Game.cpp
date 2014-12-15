#include "Precomp.h"

_Use_decl_annotations_
Game::Game(Renderer* renderer) :
    _renderer(renderer), _playerVelocity(0, 0, 0)
{
    _lastTime.QuadPart = 0;
    QueryPerformanceFrequency(&_frequency);

    _planes[0] = GeometricPrimitive::CreatePlane(renderer->Context().Get(), XMVectorSet(0, 1, 0, 0), 5, false);
    _planes[1] = GeometricPrimitive::CreatePlane(renderer->Context().Get(), XMVectorSet(1, 0, 0, 0), 5, false);
    _planes[2] = GeometricPrimitive::CreatePlane(renderer->Context().Get(), XMVectorSet(-1, 0, 0, 0), 5, false);
    _planes[3] = GeometricPrimitive::CreatePlane(renderer->Context().Get(), XMVectorSet(0, 0, -1, 0), 5, false);

    // HACK HACK
    _triangleVertices[0] = XMFLOAT3(-2.5f, 5.0f, -2.5f);
    _triangleVertices[1] = XMFLOAT3(-2.5f, 5.0f, 2.5f);
    _triangleVertices[2] = XMFLOAT3(-2.5f, 0.0f, 2.5f);
    _triangleVertices[3] = XMFLOAT3(-2.5f, 0.0f, -2.5f);
    _triangleVertices[4] = XMFLOAT3(2.5f, 5.0f, 2.5f);
    _triangleVertices[5] = XMFLOAT3(2.5f, 5.0f, -2.5f);
    _triangleVertices[6] = XMFLOAT3(2.5f, 0.0f, -2.5f);
    _triangleVertices[7] = XMFLOAT3(2.5f, 0.0f, 2.5f);

    _triangleIndices[0] = 0;
    _triangleIndices[1] = 1;
    _triangleIndices[2] = 2;
    _triangleIndices[3] = 0;
    _triangleIndices[4] = 2;
    _triangleIndices[5] = 3;
    _triangleIndices[6] = 1;
    _triangleIndices[7] = 4;
    _triangleIndices[8] = 7;
    _triangleIndices[9] = 1;
    _triangleIndices[10] = 7;
    _triangleIndices[11] = 2;
    _triangleIndices[12] = 4;
    _triangleIndices[13] = 5;
    _triangleIndices[14] = 6;
    _triangleIndices[15] = 4;
    _triangleIndices[16] = 6;
    _triangleIndices[17] = 7;
    _triangleIndices[18] = 2;
    _triangleIndices[19] = 7;
    _triangleIndices[20] = 6;
    _triangleIndices[21] = 2;
    _triangleIndices[22] = 6;
    _triangleIndices[23] = 3;

    _planeTransforms[1].SetPosition(XMFLOAT3(-2.5f, 2.5f, 0));
    _planeTransforms[2].SetPosition(XMFLOAT3(2.5f, 2.5f, 0));
    _planeTransforms[3].SetPosition(XMFLOAT3(0, 2.5f, 2.5f));

    _player = GeometricPrimitive::CreateCube(renderer->Context().Get(), 1.0f, false);

    _playerTransform.SetPosition(XMFLOAT3(0, 2, 0));

    XMStoreFloat4x4(&_view, XMMatrixLookAtLH(XMVectorSet(0, 3, -5, 0), XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 1, 0, 1)));

    // TODO: Compute aspect ratio from actual render target size
    XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), 16.0f / 9.0f, 0.01f, 1000.0f));
}

void Game::Tick()
{
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    float elapsedTime = 0.0f;

    if (_lastTime.QuadPart != 0)
    {
        elapsedTime = (float)((now.QuadPart - _lastTime.QuadPart) / (double)_frequency.QuadPart);
    }

    _lastTime = now;

    Update(elapsedTime);

    static const float clearColor[] = { 0.0f, 0.0f, 0.2f, 1.0f };
    _renderer->Clear(clearColor, 1.0f);

    Draw();

    _renderer->Present(true);
}

void Game::Update(float elapsedTime)
{
    //
    // HACK
    //
    XMFLOAT3 movement(0, 0, 0);
    float increment = elapsedTime * 2;

    if (GetKeyState(VK_LEFT) & 0x8000)
    {
        movement.x -= increment;
    }
    if (GetKeyState(VK_RIGHT) & 0x8000)
    {
        movement.x += increment;
    }
    if (GetKeyState(VK_UP) & 0x8000)
    {
        movement.z += increment;
    }
    if (GetKeyState(VK_DOWN) & 0x8000)
    {
        movement.z -= increment;
    }

    HandleMovement(movement);

    static const float Gravity = 1.0f;
    movement = XMFLOAT3(0, -Gravity * elapsedTime, 0);

    HandleMovement(movement);
}

void Game::Draw()
{
    for (uint32_t i = 0; i < _countof(_planes); ++i)
    {
        _planes[i]->Draw(XMLoadFloat4x4(&_planeTransforms[i].World()), XMLoadFloat4x4(&_view), XMLoadFloat4x4(&_projection), Colors::Gray);
    }
    _player->Draw(XMLoadFloat4x4(&_playerTransform.World()), XMLoadFloat4x4(&_view), XMLoadFloat4x4(&_projection), Colors::Blue);
}

void Game::HandleMovement(const XMFLOAT3& movement)
{
    XMFLOAT3 position = _playerTransform.Position();
    XMFLOAT3 vel = movement;

#if 0
    SphereSupportMapping playerMapping;
    playerMapping.Center.v = XMVectorZero();
    playerMapping.Radius = 0.5f;
#endif
    CubeSupportMapping playerMapping;
    playerMapping.Center.v = XMVectorZero();
    playerMapping.HalfWidths.f[0] = 0.5f;
    playerMapping.HalfWidths.f[1] = 0.5f;
    playerMapping.HalfWidths.f[2] = 0.5f;

    bool hit = false;
    uint32_t iterations = 0;

    while (vel.x * vel.x + vel.y * vel.y + vel.z * vel.z > 0 && iterations++ < 10)
    {
        XMFLOAT3 normal(0, 0, 0);
        float distance = 0.0f;

        XMFLOAT3 minNormal(0, 0, 0);
        float minDistance = FLT_MAX;

        hit = false;

        for (uint32_t tri = 0; tri < _countof(_planes) * 2; ++tri)
        {
            TriangleSupportMapping triangle;

            uint32_t count = 0;
            for (uint32_t i = tri * 3; i < tri * 3 + 3; ++i)
            {
                triangle.Points[count++].v = XMLoadFloat3(&_triangleVertices[_triangleIndices[i]]);
            }

            if (ShapeCast(position, vel, &playerMapping,
                            XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), &triangle,
                            &normal, &distance))
            {
                if (distance < minDistance)
                {
                    minDistance = distance;
                    minNormal = normal;
                    hit = true;

                    // HACK (what happens if we use face normals instead of true normals? seems to be better)
                    XMStoreFloat3(&minNormal, XMVector3Normalize(XMVector3Cross(triangle.Points[1].v - triangle.Points[0].v, triangle.Points[2].v - triangle.Points[0].v)));
                }
            }
        }

        if (hit)
        {
            XMVECTOR move = XMLoadFloat3(&vel) * (minDistance - 0.00001f);
            XMStoreFloat3(&position, XMLoadFloat3(&position) + move);

            XMVECTOR remainder = XMLoadFloat3(&vel) - move;
            remainder += XMLoadFloat3(&minNormal) * -XMVector3Dot(XMLoadFloat3(&normal), remainder);
            XMStoreFloat3(&vel, remainder);
        }
        else
        {
            XMVECTOR move = XMLoadFloat3(&vel);
            XMStoreFloat3(&position, XMLoadFloat3(&position) + move);
            break;
        }
    }

    _playerTransform.SetPosition(position);
    _playerVelocity = vel;
}
