#pragma once

//
// Implementation of matte material, using Lambertian reflectance.
//
class Lambert : public Material, public TrackedObject<MemoryTag::Material>
{
public:
    Lambert();

    void Draw(_In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection, _In_count_(numObjects) GameObject** objects, _In_ uint32_t numObjects) override;

private:
    Lambert(const Lambert&);

private:
    //
    // Constant buffer structs
    //
    struct PerFrame
    {
        XMFLOAT4X4 View;
        XMFLOAT4X4 Projection;
    };

    struct PerObject
    {
        XMFLOAT4X4 World;
        uint32_t TexIndex;
        XMUINT3 Padding;
    };

private:
    PerFrame _perFrame;
    PerObject _perObject;

    ComPtr<ID3D11Buffer> _perFrameCB;
    ComPtr<ID3D11Buffer> _perObjectCB;
};
