#pragma once

#include "primitives.h"
#include <map>

class Renderer;

struct LightPatch
{
    // TODO: possibly move position & normal off to a throwaway struct. They're only used during
    // the initial building of patches, and just waste space after that

    DirectX::XMFLOAT3   position;       // world position
    DirectX::XMFLOAT3   normal;         // world normal

    DirectX::XMFLOAT3   emission;       // how much light is coming out of this patch
    DirectX::XMFLOAT3   incident;       // sum of incoming light from all the patches we can see
    DirectX::XMFLOAT3   excident;       // total light given off from this patch. (incident * reflectance) + emission
    float               reflectance;    // how much light is reflected back out

    std::map<uint32_t, float> factors;
};

class PatchManager
{
public:
    PatchManager(_In_ const Microsoft::WRL::ComPtr<ID3D11Device>& device);

    void BeginPatchMap(_In_ float patchSizeWorld, _In_ uint32_t mapSizePixels);

    // Updates the UV coordinates in the quad to reflect the location in the map.
    void InsertQuad(_Inout_ Quad& quad, _In_ const DirectX::XMFLOAT4X4& world, _In_ const DirectX::XMFLOAT3& emission);

    void EndPatchMap();

    void GeneratePatchViews(_In_ Renderer* renderer);

    const Microsoft::WRL::ComPtr<ID3D11Texture2D>& GetLightMap() const;

private:
    struct Patch
    {
        uint32_t x, y; // in light map
        float emission;
        float reflectance;
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT3 up;
    };

    void BuildMuliplierMap();
    void RenderPatchViews(_In_ Renderer* renderer, _In_ Patch* patch);
    void ComputeLight(_In_ Patch* patch);

private:
    float _patchSizeWorld;      // world space size of a patch
    uint32_t _mapSizePixels;    // number of pixels of each dimension of light map
    uint32_t _patchViewSize;    // size of the surfaces used to render the patch views

    // For patch view rendering
    Microsoft::WRL::ComPtr<ID3D11Device> _device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> _textures[5];
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _rtv[5];
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _depthStencil;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> _patchView;

    std::vector<Patch> _patches;
    std::unique_ptr<float[]> _lightMapData;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> _lightMap;

    struct PatchBuilder
    {
        std::vector<RECT> rects;
        std::unique_ptr<uint32_t[]> patchMap;

        uint32_t currentX;
        uint32_t currentY;
        uint32_t currentHeight;
    };

    std::unique_ptr<PatchBuilder> _patchBuilder;

    std::unique_ptr<double[]> _multiplierMap;
};
