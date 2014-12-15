#pragma once

#include "..\Effect.h"

#include <d3d11.h>
#include <wrl\client.h>

using Microsoft::WRL::ComPtr;

namespace GDK
{
    class DxEffect : public Effect
    {
    public:
        static std::shared_ptr<Effect> Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ Effect::Type type);

        virtual void Apply() override;

    protected:
        DxEffect(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice);
        virtual ~DxEffect();

        void CreateVertexShaderAndInputLayout(_In_ const wchar_t* filename, _In_ const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements);
        void CreatePixelShader(_In_ const wchar_t* filename);

        void CreateSampler(_In_ FilterType filter, _In_ AddressMode addressU, _In_ AddressMode addressV);
        void CreateBlendState(_In_ BlendMode blendMode);

        void CreateVSConstantBuffer(_In_ uint32_t numBytes);
        void CreatePSConstantBuffer(_In_ uint32_t numBytes);

        void UpdateVSConstantBuffer(_In_ const void* data, _In_ uint32_t numBytes);
        void UpdatePSConstantBuffer(_In_ const void* data, _In_ uint32_t numBytes);

        void DrawGeometry(_In_ const std::shared_ptr<Geometry>& geometry, _In_ uint32_t frame);

    private:
        std::unique_ptr<byte_t[]> ReadShaderFile(_In_ const wchar_t* filename, _Out_ uint32_t* fileLength);

        ComPtr<ID3D11DeviceContext> _context;
        ComPtr<ID3D11VertexShader> _vertexShader;
        ComPtr<ID3D11PixelShader> _pixelShader;
        ComPtr<ID3D11InputLayout> _inputLayout;
        ComPtr<ID3D11SamplerState> _sampler;
        ComPtr<ID3D11BlendState> _blendState;
        ComPtr<ID3D11Buffer> _vsConstantBuffer;
        ComPtr<ID3D11Buffer> _psConstantBuffer;
    };
}
