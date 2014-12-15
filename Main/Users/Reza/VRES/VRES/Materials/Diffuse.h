#pragma once

#include "..\Material.h"

namespace VRES
{
    class DiffuseMaterial : NonCopyable<DiffuseMaterial>, public Material
    {
    public:
        static std::shared_ptr<Material> Create(_In_ VRES::Renderer* renderer, const ComPtr<ID3D11DeviceContext>& context);

    private:
        DiffuseMaterial(_In_ VRES::Renderer* renderer, const ComPtr<ID3D11DeviceContext>& context);

        bool Initialize();
        void RenderModels(const OutputDescription& output, _In_count_(numModels) const Model* const* models, uint32_t numModels, _In_count_(numLights) const Light* const* lights, uint32_t numLights) override;

    private:
        struct FrameConstants
        {
            XMFLOAT4X4 View;
            XMFLOAT4X4 Projection;
        };

        struct ObjectConstants
        {
            XMFLOAT4X4 World;
        };

        struct LightConstant
        {
            XMFLOAT3 Position;
            uint32_t Type;
            XMFLOAT3 Color;
            float Radius;
        };

        static const uint32_t MAX_LIGHTS = 8;

        struct LightConstants
        {
            LightConstant Lights[MAX_LIGHTS];
            uint32_t NumLights;
            XMFLOAT3 CameraPosition;
        };
    };
}
