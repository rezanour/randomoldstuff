#include "StdAfx.h"
#include "DirectionalLightShader.h"
#include "Renderer.h"
#include "RenderTask.h"

#include <CoreServices\FileStream.h>
#include <CoreServices\StreamHelper.h>

#include <D3Dcompiler.h>

using namespace Lucid3D;
using namespace L3DMath;
using namespace CoreServices;

DirectionalLightShader::DirectionalLightShader()
{
}

DirectionalLightShader::~DirectionalLightShader()
{
}

GDK_METHODIMP DirectionalLightShader::Initialize(_In_ Renderer* pRenderer)
{
    HRESULT hr = S_OK;

    DevicePtr spDevice = pRenderer->GetDevice();
    stde::com_ptr<IStream> spStream;

    uint64 length = 0;

    // TODO: Fill in real material table. For now let's just
    // assume material 0xffffffff is the directional light, and that we support it well
    AddSupportedMaterial(0xffffffff, 100);

    CHECKHR(FileStream::Create(L"DirectionalLight.hlsl", true, &spStream));
    length = FileStream::GetLength(spStream);
    ISTRUE(length > 0, E_FAIL);

    {
        std::vector<byte> buffer(length);
        ULONG cbRead = 0;
        stde::com_ptr<ID3DBlob> spCode, spErrors;

        D3D11_BUFFER_DESC bufferDesc = {0};
        D3D11_INPUT_ELEMENT_DESC inputElem = {0};
        D3D11_BLEND_DESC blendDesc = {0};

        std::vector<D3D11_INPUT_ELEMENT_DESC> inputElems;

        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
        dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

        CHECKHR(spStream->Read(buffer.data(), buffer.size(), &cbRead));
        hr = D3DCompile(buffer.data(), buffer.size(), nullptr, nullptr, nullptr, "vsFullscreenQuad", "vs_4_0", dwShaderFlags, 0, &spCode, &spErrors);
        if (FAILED(hr))
        {
            DebugOut("Error compiling shader:\n\n%s\n", static_cast<PSTR>(spErrors->GetBufferPointer()));
            CHECKHR(hr);
        }

        CHECKHR(spDevice->CreateVertexShader(spCode->GetBufferPointer(), spCode->GetBufferSize(), nullptr, &_spQuadVertexShader));
        // Store off the shader code for input layout construction later
        SetShaderCode(spCode);

        spCode = nullptr;
        spErrors = nullptr;
        hr = D3DCompile(buffer.data(), buffer.size(), nullptr, nullptr, nullptr, "psDirectionalLight", "ps_4_0", dwShaderFlags, 0, &spCode, &spErrors);
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
            std::string name("DirectionalLightShader::FrameConstantBuffer");
            _spFrameConstantBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
        }

        // Fill in the screen size (only needed once for now, unless we support dynamic resize)
        _frameConstants.ScreenSize.Set(
            pRenderer->GetSettings().DefaultView.ScreenWidth, 
            pRenderer->GetSettings().DefaultView.ScreenHeight);

        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.ByteWidth = sizeof(_lightConstants);
        bufferDesc.StructureByteStride = sizeof(_lightConstants);
        CHECKHR(spDevice->CreateBuffer(&bufferDesc, nullptr, &_spLightConstantBuffer));
        {
            std::string name("DirectionalLightShader::LightConstantBuffer");
            _spFrameConstantBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
        }

        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_ALPHA;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        CHECKHR(spDevice->CreateBlendState(&blendDesc, &_spBlendState));
    }

EXIT
    return hr;
}

GDK_METHODIMP DirectionalLightShader::RenderBatch(_In_ ContextPtr& spContext, _In_ CameraPtr& spCamera, _In_ std::vector<RenderTaskPtr>& renderTasks)
{
    HRESULT hr = S_OK;

    SetupPerFrame(spContext, spCamera);

    float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    spContext->OMSetBlendState(_spBlendState, blendFactor, 0xffffffff);

    // doesn't handle instancing, so do each one at a time
    for (size_t i = 0; i < renderTasks.size(); i++)
    {
        RenderTaskPtr task = renderTasks[i];

        if (task->Type == TaskType::LightTask)
        {
            LightRenderTask* lightTask = reinterpret_cast<LightRenderTask*>(task.get());
            _lightConstants.LightDirection = GDK::GetForward(DirectX::XMFLOAT4X4(task->Matrix));
            _lightConstants.LightColor = lightTask->Light->GetColor();

            // NOTE: Assumes that quad geometry is already FULL SCREEN!!
            // EX:
            //           (-1, 1) ---- (1, 1)
            //              |            |
            //           (-1, -1)---- (1, -1)
            //

            // draw the geometry
            CHECKHR(task->Geometry->Draw(GetId(), spContext));
        }
    }

    spContext->OMSetBlendState(nullptr, blendFactor, 0xffffffff);

EXIT
    return hr;
}

void DirectionalLightShader::SetupPerFrame(_In_ ContextPtr& spContext, _In_ CameraPtr& spCamera)
{
    if (!spCamera)
        return;

    // per frame stuff
    DirectX::XMVECTOR det;
    _frameConstants.InvViewProjMatrix = Matrix::Transpose(DirectX::XMMatrixInverse(&det, spCamera->GetViewMatrix() * spCamera->GetProjectionMatrix()));
    _frameConstants.CameraPosition = GDK::GetEyePoint(spCamera->GetViewMatrix());

    // update hardware constant buffer
    spContext->UpdateSubresource(_spFrameConstantBuffer, 0, nullptr, &_frameConstants, sizeof(_frameConstants), sizeof(_frameConstants));

    // update hardware constant buffer
    spContext->UpdateSubresource(_spLightConstantBuffer, 0, nullptr, &_lightConstants, sizeof(_lightConstants), sizeof(_lightConstants));

    // Both shaders need access to the constant buffer
    spContext->VSSetConstantBuffers(0, 1, &_spFrameConstantBuffer);
    spContext->PSSetConstantBuffers(0, 1, &_spFrameConstantBuffer);

    // Both shaders need access to the constant buffer
    spContext->VSSetConstantBuffers(1, 1, &_spLightConstantBuffer);
    spContext->PSSetConstantBuffers(1, 1, &_spLightConstantBuffer);

    // set shaders on the device
    spContext->VSSetShader(_spQuadVertexShader, nullptr, 0);
    spContext->PSSetShader(_spPixelShader, nullptr, 0);
}

