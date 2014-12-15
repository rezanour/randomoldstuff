#include "DxGraphicsDevice.h"
#include "DxEffect.h"
#include "DxEffectStatic3D.h"
#include "DxEffectDraw2D.h"

#include "DxGeometry.h"

#include <GDKError.h>

#include <wrl\wrappers\corewrappers.h>

using Microsoft::WRL::Wrappers::FileHandle;

namespace GDK
{
    std::shared_ptr<Effect> DxEffect::Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ Effect::Type type)
    {
        switch (type)
        {
        case Type::StaticGeometry3D:
            return DxEffectStatic3D::Create(graphicsDevice);
            break;

        case Type::Draw2D:
            return DxEffectDraw2D::Create(graphicsDevice);
            break;

        default:
            throw std::exception();
        }
    }

    DxEffect::DxEffect(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice) :
        Effect(graphicsDevice)
    {
        auto graphics = LockGraphicsDevice();
        _context = static_cast<DxGraphicsDevice*>(graphics.get())->GetContext();
    }

    DxEffect::~DxEffect()
    {
    }

    void DxEffect::Apply()
    {
        _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
        _context->PSSetShader(_pixelShader.Get(), nullptr, 0);
        _context->IASetInputLayout(_inputLayout.Get());
        _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        _context->PSSetSamplers(0, 1, _sampler.GetAddressOf());
        _context->OMSetBlendState(_blendState.Get(), nullptr, 0xffffffff);
        _context->VSSetConstantBuffers(0, 1, _vsConstantBuffer.GetAddressOf());
        _context->PSSetConstantBuffers(0, 1, _psConstantBuffer.GetAddressOf());
    }

    void DxEffect::CreateVertexShaderAndInputLayout(_In_ const wchar_t* filename, _In_ const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements)
    {
        ComPtr<ID3D11Device> device;
        _context->GetDevice(&device);

        uint32_t fileLength;
        auto shaderByteCode = ReadShaderFile(filename, &fileLength);

        CHECK_HR(device->CreateVertexShader(shaderByteCode.get(), fileLength, nullptr, _vertexShader.ReleaseAndGetAddressOf()));
        CHECK_HR(device->CreateInputLayout(inputElements.data(), static_cast<UINT>(inputElements.size()), shaderByteCode.get(), fileLength, _inputLayout.ReleaseAndGetAddressOf()));
    }

    void DxEffect::CreatePixelShader(_In_ const wchar_t* filename)
    {
        ComPtr<ID3D11Device> device;
        _context->GetDevice(&device);

        uint32_t fileLength;
        auto shaderByteCode = ReadShaderFile(filename, &fileLength);

        CHECK_HR(device->CreatePixelShader(shaderByteCode.get(), fileLength, nullptr, _pixelShader.ReleaseAndGetAddressOf()));
    }

    void DxEffect::CreateSampler(_In_ FilterType filter, _In_ AddressMode addressU, _In_ AddressMode addressV)
    {
        ComPtr<ID3D11Device> device;
        _context->GetDevice(&device);

        D3D11_SAMPLER_DESC desc = {};
        desc.AddressU = (addressU == AddressMode::Wrap) ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = (addressV == AddressMode::Wrap) ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.Filter = (filter == FilterType::Linear) ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;

        CHECK_HR(device->CreateSamplerState(&desc, _sampler.ReleaseAndGetAddressOf()));
    }

    void DxEffect::CreateBlendState(_In_ BlendMode blendMode)
    {
        if (blendMode == BlendMode::Opaque)
        {
            // default is opaque
            _blendState = nullptr;
            return;
        }

        ComPtr<ID3D11Device> device;
        _context->GetDevice(&device);

        D3D11_BLEND_DESC desc = {};
        desc.RenderTarget[0].BlendEnable = TRUE;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        CHECK_HR(device->CreateBlendState(&desc, _blendState.ReleaseAndGetAddressOf()));
    }

    void DxEffect::CreateVSConstantBuffer(_In_ uint32_t numBytes)
    {
        ComPtr<ID3D11Device> device;
        _context->GetDevice(&device);

        D3D11_BUFFER_DESC desc = {};
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.ByteWidth = numBytes;
        desc.StructureByteStride = numBytes;
        desc.Usage = D3D11_USAGE_DEFAULT;

        CHECK_HR(device->CreateBuffer(&desc, nullptr, _vsConstantBuffer.ReleaseAndGetAddressOf()));
    }

    void DxEffect::CreatePSConstantBuffer(_In_ uint32_t numBytes)
    {
        ComPtr<ID3D11Device> device;
        _context->GetDevice(&device);

        D3D11_BUFFER_DESC desc = {};
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.ByteWidth = numBytes;
        desc.StructureByteStride = numBytes;
        desc.Usage = D3D11_USAGE_DEFAULT;

        CHECK_HR(device->CreateBuffer(&desc, nullptr, _psConstantBuffer.ReleaseAndGetAddressOf()));
    }

    void DxEffect::UpdateVSConstantBuffer(_In_ const void* data, _In_ uint32_t numBytes)
    {
        _context->UpdateSubresource(_vsConstantBuffer.Get(), 0, nullptr, data, numBytes, 0);
    }

    void DxEffect::UpdatePSConstantBuffer(_In_ const void* data, _In_ uint32_t numBytes)
    {
        _context->UpdateSubresource(_psConstantBuffer.Get(), 0, nullptr, data, numBytes, 0);
    }

    void DxEffect::DrawGeometry(_In_ const std::shared_ptr<Geometry>& geometry, _In_ uint32_t frame)
    {
        auto dxGeometry = static_cast<DxGeometry*>(geometry.get());
        dxGeometry->Draw(frame);
    }

    std::unique_ptr<byte_t[]> DxEffect::ReadShaderFile(_In_ const wchar_t* filename, _Out_ uint32_t* fileLength)
    {
        FileHandle shaderFile(CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
        CHECK_WIN32(shaderFile.IsValid());

        BY_HANDLE_FILE_INFORMATION info = {};
        CHECK_WIN32(GetFileInformationByHandle(shaderFile.Get(), &info));

        *fileLength = info.nFileSizeLow;
        std::unique_ptr<byte[]> shaderByteCode(new byte[*fileLength]);

        DWORD bytesRead = 0;
        CHECK_WIN32(ReadFile(shaderFile.Get(), shaderByteCode.get(), *fileLength, &bytesRead, nullptr));

        return shaderByteCode;
    }
}
