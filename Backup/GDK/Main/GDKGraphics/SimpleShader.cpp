#include "StdAfx.h"
#include "SimpleShader.h"
#include "Renderer.h"
#include "RenderTask.h"

#include <CoreServices\FileStream.h>
#include <CoreServices\StreamHelper.h>

#include <D3Dcompiler.h>

using namespace Lucid3D;
using namespace CoreServices;

SimpleShader::SimpleShader()
{
}

SimpleShader::~SimpleShader()
{
}

GDK_IMETHODIMP SimpleShader::Initialize(_In_ Renderer* pRenderer)
{
    HRESULT hr = S_OK;

    DevicePtr spDevice = pRenderer->GetDevice();
    stde::com_ptr<IStream> spStream;

    uint64 length = 0;

    // TODO: Fill in real material table. For now let's just
    // assume material 0 is the only one, and that we support it well
    AddSupportedMaterial(0, 100);

    CHECKHR(FileStream::Create(L"SimpleShader.hlsl", true, &spStream));
    length = FileStream::GetLength(spStream);
    ISTRUE(length > 0, E_FAIL);

    {
        std::vector<byte> buffer(length);
        ULONG cbRead = 0;
        stde::com_ptr<ID3DBlob> spCode, spErrors;

        D3D11_BUFFER_DESC bufferDesc = {0};
        D3D11_INPUT_ELEMENT_DESC inputElem = {0};
        D3D11_SAMPLER_DESC samplerDesc;

        ZeroMemory(&samplerDesc, sizeof(samplerDesc));

        std::vector<D3D11_INPUT_ELEMENT_DESC> inputElems;

        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
        // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
        // Setting this flag improves the shader debugging experience, but still allows 
        // the shaders to be optimized and to run exactly the way they will run in 
        // the release configuration of this program.
        dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

        CHECKHR(spStream->Read(buffer.data(), buffer.size(), &cbRead));
        hr = D3DCompile(buffer.data(), buffer.size(), nullptr, nullptr, nullptr, "vsSimpleShader", "vs_4_0", dwShaderFlags, 0, &spCode, &spErrors);
        if (FAILED(hr))
        {
            DebugOut("Error compiling shader:\n\n%s\n", static_cast<PSTR>(spErrors->GetBufferPointer()));
            CHECKHR(hr);
        }

        CHECKHR(spDevice->CreateVertexShader(spCode->GetBufferPointer(), spCode->GetBufferSize(), nullptr, &_spVertexShader));
        // Store off the shader code for input layout construction later
        SetShaderCode(spCode);

        spCode = nullptr;
        spErrors = nullptr;
        hr = D3DCompile(buffer.data(), buffer.size(), nullptr, nullptr, nullptr, "psSimpleShader", "ps_4_0", dwShaderFlags, 0, &spCode, &spErrors);
        if (FAILED(hr))
        {
            DebugOut("Error compiling shader:\n\n%s\n", static_cast<PSTR>(spErrors->GetBufferPointer()));
            CHECKHR(hr);
        }

        CHECKHR(spDevice->CreatePixelShader(spCode->GetBufferPointer(), spCode->GetBufferSize(), nullptr, &_spPixelShader));

        // build constant buffers

        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.ByteWidth = sizeof(_frameConstants);
        bufferDesc.StructureByteStride = sizeof(_frameConstants);
        CHECKHR(spDevice->CreateBuffer(&bufferDesc, nullptr, &_spFrameConstantBuffer));
        {
            std::string name("SimpleShader::FrameConstantBuffer");
            _spFrameConstantBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
        }

        bufferDesc.ByteWidth = sizeof(_objectConstants);
        bufferDesc.StructureByteStride = sizeof(_objectConstants);
        CHECKHR(spDevice->CreateBuffer(&bufferDesc, nullptr, &_spObjectConstantBuffer));
        {
            std::string name("SimpleShader::ObjectConstantBuffer");
            _spObjectConstantBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
        }

        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        CHECKHR(spDevice->CreateSamplerState(&samplerDesc, &_spLinearSampler));

        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        CHECKHR(spDevice->CreateSamplerState(&samplerDesc, &_spPointSampler));
    }

EXIT
    return hr;
}

GDK_IMETHODIMP SimpleShader::RenderBatch(_In_ ContextPtr& spContext, _In_ CameraPtr& spCamera, _In_ std::vector<RenderTaskPtr>& renderTasks)
{
    HRESULT hr = S_OK;

    SetupPerFrame(spContext, spCamera);

    // doesn't handle instancing, so do each one at a time
    for (size_t i = 0; i < renderTasks.size(); i++)
    {
        RenderTaskPtr task = renderTasks[i];

        // per object stuff
        _objectConstants.WorldMatrix = Matrix::Transpose(task->Matrix);

        TexturePtr diffuse = task->Material->GetTexture(TextureUsage::Diffuse);
        TexturePtr normalMap = task->Material->GetTexture(TextureUsage::NormalMap);

        if (diffuse)
            diffuse->BindTexture(spContext, 0);

        if (normalMap)
            normalMap->BindTexture(spContext, 1);

        // update hardware constant buffer
        spContext->UpdateSubresource(_spObjectConstantBuffer, 0, nullptr, &_objectConstants, sizeof(_objectConstants), sizeof(_objectConstants));

        // Only vertex shader needs this constant buffer
        spContext->VSSetConstantBuffers(1, 1, &_spObjectConstantBuffer);

        // draw the geometry
        CHECKHR(task->Geometry->Draw(GetId(), spContext));
    }

EXIT
    return hr;
}

void SimpleShader::SetupPerFrame(_In_ ContextPtr& spContext, _In_ CameraPtr& spCamera)
{
    if (!spCamera)
        return;

    // per frame stuff
    _frameConstants.ViewProjMatrix = Matrix::Transpose(spCamera->GetViewMatrix() * spCamera->GetProjectionMatrix());
    _frameConstants.CameraPosition = GDK::GetEyePoint(spCamera->GetViewMatrix());
    _frameConstants.OneOverFarClipDistance = 1.0f / spCamera->GetFarClipPlane();

    // update hardware constant buffer
    spContext->UpdateSubresource(_spFrameConstantBuffer, 0, nullptr, &_frameConstants, sizeof(_frameConstants), sizeof(_frameConstants));

    // Both shaders need access to the constant buffer
    spContext->VSSetConstantBuffers(0, 1, &_spFrameConstantBuffer);
    spContext->PSSetConstantBuffers(0, 1, &_spFrameConstantBuffer);

    // set shaders on the device
    spContext->VSSetShader(_spVertexShader, nullptr, 0);
    spContext->PSSetShader(_spPixelShader, nullptr, 0);

    // set up linear for both diffuse & normals
    ID3D11SamplerState* samplers[] = { _spLinearSampler, _spLinearSampler };
    spContext->VSSetSamplers(0, _countof(samplers), samplers);
    spContext->PSSetSamplers(0, _countof(samplers), samplers);

}

