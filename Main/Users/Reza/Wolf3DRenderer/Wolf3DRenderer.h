#pragma once

struct IWolf3DRenderer
{
    // Force delete to do proper destruction of derived classes
    virtual ~IWolf3DRenderer() {}

    //
    // This just needs to be called once to load the data
    //
    virtual HRESULT LoadTextures(const uint8_t* vswapData, uint32_t size) = 0;

    //
    // These can just be called on first init & level transitions.
    //
    virtual HRESULT SetColors(const XMFLOAT3& ceiling, const XMFLOAT3& floor) = 0;
    virtual HRESULT SetWallLayer(uint32_t width, uint32_t height, _In_count_(width * height) const uint16_t* uncompressedWallLayer) = 0;

    //
    // Called each frame to draw sprites
    //
    virtual void DrawSprite3D(const XMFLOAT2& positionXZ, uint32_t spriteIndex) = 0;

    //
    // Called to render 3D view of the level to the output surface provided at creation time.
    // The output is scaled to fit into the viewport provided.
    //
    virtual HRESULT Render(const XMFLOAT4X4& view, const XMFLOAT4X4& projection, const RECT& viewport) = 0;
};

//
// Direct3D based implementation
//
// renderTarget is the surface that the Render call will target once invoked.
//
HRESULT CreateWolf3DRendererDx(_In_ ID3D11Device* device, _In_ ID3D11RenderTargetView* renderTarget, _Outptr_ IWolf3DRenderer** renderer);

//
// Software Ray-casting implementation
//
HRESULT CreateWolf3DRendererSoftware(uint32_t width, uint32_t height, _In_count_(width * height) uint32_t* surface, _Outptr_ IWolf3DRenderer** renderer);
