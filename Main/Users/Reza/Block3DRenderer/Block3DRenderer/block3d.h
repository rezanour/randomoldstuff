#pragma once

struct IBlock3DRenderer
{
    virtual ~IBlock3DRenderer() {}

    virtual void SetLevelColors(const XMFLOAT3& ceiling, const XMFLOAT3& floor) = 0;

    virtual void SetWallLayout(_In_count_(64 * 64) const int16_t* data) = 0;

    // Render setup. Saves off current pipeline state and sets up camera.
    virtual void Begin(const D3D11_VIEWPORT& viewport, const XMFLOAT4X4& view, const XMFLOAT4X4& projection, bool preservePipelineState) = 0;

    // Draw a billboarded sprite
    virtual void DrawSprite(const XMFLOAT2& position, uint16_t spriteIndex) = 0;

    // Draw a dynamic wall cube at non-aligned position (secrets, etc...)
    virtual void DrawCube(const XMFLOAT2& position, uint16_t wallIndex) = 0;

    // Draw a dynamic door
    virtual void DrawDoor(const XMFLOAT2& position, bool horizontal, uint16_t imageIndex) = 0;

    // Complete rendering.
    virtual void End() = 0;
};

IBlock3DRenderer* CreateBlock3DRenderer(_In_ ID3D11Device* device, _In_ ID3D11ShaderResourceView* walls, _In_ ID3D11ShaderResourceView* sprites, bool useLinearFiltering = false);
