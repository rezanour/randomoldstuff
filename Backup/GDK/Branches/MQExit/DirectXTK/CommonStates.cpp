// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

#include "pch.h"
#include "CommonStates.h"
#include "SharedResourcePool.h"
#include "PlatformHelpers.h"

using namespace DirectX;
using namespace Microsoft::WRL;


// Internal state object implementation class. Only one of these helpers is allocated
// per D3D device, even if there are multiple public facing CommonStates instances.
class CommonStates::Impl
{
public:
    Impl(_In_ ID3D11Device* device)
      : device(device)
    { }

    void CreateBlendState(D3D11_BLEND srcBlend, D3D11_BLEND destBlend, _Out_ ID3D11BlendState** pResult);
    void CreateDepthStencilState(bool enable, bool writeEnable, _Out_ ID3D11DepthStencilState** pResult);
    void CreateRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, _Out_ ID3D11RasterizerState** pResult);
    void CreateSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode, _Out_ ID3D11SamplerState** pResult);

    ComPtr<ID3D11Device> device;

    ComPtr<ID3D11BlendState> opaque;
    ComPtr<ID3D11BlendState> alphaBlend;
    ComPtr<ID3D11BlendState> additive;
    ComPtr<ID3D11BlendState> nonPremultiplied;

    ComPtr<ID3D11DepthStencilState> depthNone;
    ComPtr<ID3D11DepthStencilState> depthDefault;
    ComPtr<ID3D11DepthStencilState> depthRead;

    ComPtr<ID3D11RasterizerState> cullNone;
    ComPtr<ID3D11RasterizerState> cullClockwise;
    ComPtr<ID3D11RasterizerState> cullCounterClockwise;
    ComPtr<ID3D11RasterizerState> wireframe;

    ComPtr<ID3D11SamplerState> pointWrap;
    ComPtr<ID3D11SamplerState> pointClamp;
    ComPtr<ID3D11SamplerState> linearWrap;
    ComPtr<ID3D11SamplerState> linearClamp;
    ComPtr<ID3D11SamplerState> anisotropicWrap;
    ComPtr<ID3D11SamplerState> anisotropicClamp;

    static SharedResourcePool<ID3D11Device*, Impl> instancePool;
};


// Global instance pool.
SharedResourcePool<ID3D11Device*, CommonStates::Impl> CommonStates::Impl::instancePool;


// Helper for creating blend state objects.
void CommonStates::Impl::CreateBlendState(D3D11_BLEND srcBlend, D3D11_BLEND destBlend, _Out_ ID3D11BlendState** pResult)
{
    D3D11_BLEND_DESC desc;
    ZeroMemory(&desc, sizeof(desc));

    desc.RenderTarget[0].BlendEnable = (srcBlend != D3D11_BLEND_ONE) ||
                                       (destBlend != D3D11_BLEND_ZERO);

    desc.RenderTarget[0].SrcBlend  = desc.RenderTarget[0].SrcBlendAlpha  = srcBlend;
    desc.RenderTarget[0].DestBlend = desc.RenderTarget[0].DestBlendAlpha = destBlend;
    desc.RenderTarget[0].BlendOp   = desc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;

    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    ThrowIfFailed(
        device->CreateBlendState(&desc, pResult)
    );
}


// Helper for creating depth stencil state objects.
void CommonStates::Impl::CreateDepthStencilState(bool enable, bool writeEnable, _Out_ ID3D11DepthStencilState** pResult)
{
    D3D11_DEPTH_STENCIL_DESC desc;
    ZeroMemory(&desc, sizeof(desc));

    desc.DepthEnable = enable;
    desc.DepthWriteMask = writeEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    desc.StencilEnable = false;
    desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

    desc.BackFace = desc.FrontFace;

    ThrowIfFailed(
        device->CreateDepthStencilState(&desc, pResult)
    );
}


// Helper for creating rasterizer state objects.
void CommonStates::Impl::CreateRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, _Out_ ID3D11RasterizerState** pResult)
{
    D3D11_RASTERIZER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));

    desc.CullMode = cullMode;
    desc.FillMode = fillMode;
    desc.DepthClipEnable = true;
    desc.MultisampleEnable = true;

    ThrowIfFailed(
        device->CreateRasterizerState(&desc, pResult)
    );
}


// Helper for creating sampler state objects.
void CommonStates::Impl::CreateSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode, _Out_ ID3D11SamplerState** pResult)
{
    D3D11_SAMPLER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));

    desc.Filter = filter;

    desc.AddressU = addressMode;
    desc.AddressV = addressMode;
    desc.AddressW = addressMode;

    desc.MaxLOD = FLT_MAX;
    desc.MaxAnisotropy = 16;
    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

    ThrowIfFailed(
        device->CreateSamplerState(&desc, pResult)
    );
}


// Public constructor.
CommonStates::CommonStates(_In_ ID3D11Device* device)
  : pImpl(Impl::instancePool.DemandCreate(device))
{
}


// Move constructor.
CommonStates::CommonStates(CommonStates&& moveFrom)
  : pImpl(std::move(moveFrom.pImpl))
{
}


// Move assignment.
CommonStates& CommonStates::operator= (CommonStates&& moveFrom)
{
    pImpl = std::move(moveFrom.pImpl);
    return *this;
}


// Public destructor.
CommonStates::~CommonStates()
{
}


ID3D11BlendState* CommonStates::Opaque()
{
    if (!pImpl->opaque)
    {
        pImpl->CreateBlendState(D3D11_BLEND_ONE, D3D11_BLEND_ZERO, &pImpl->opaque);
    }

    return pImpl->opaque.Get();
}


ID3D11BlendState* CommonStates::AlphaBlend()
{
    if (!pImpl->alphaBlend)
    {
        pImpl->CreateBlendState(D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA, &pImpl->alphaBlend);
    }

    return pImpl->alphaBlend.Get();
}


ID3D11BlendState* CommonStates::Additive()
{
    if (!pImpl->additive)
    {
        pImpl->CreateBlendState(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_ONE, &pImpl->additive);
    }

    return pImpl->additive.Get();
}


ID3D11BlendState* CommonStates::NonPremultiplied()
{
    if (!pImpl->nonPremultiplied)
    {
        pImpl->CreateBlendState(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, &pImpl->nonPremultiplied);
    }

    return pImpl->nonPremultiplied.Get();
}


ID3D11DepthStencilState* CommonStates::DepthNone()
{
    if (!pImpl->depthNone)
    {
        pImpl->CreateDepthStencilState(false, false, &pImpl->depthNone);
    }

    return pImpl->depthNone.Get();
}


ID3D11DepthStencilState* CommonStates::DepthDefault()
{
    if (!pImpl->depthDefault)
    {
        pImpl->CreateDepthStencilState(true, true, &pImpl->depthDefault);
    }

    return pImpl->depthDefault.Get();
}


ID3D11DepthStencilState* CommonStates::DepthRead()
{
    if (!pImpl->depthRead)
    {
        pImpl->CreateDepthStencilState(true, false, &pImpl->depthRead);
    }

    return pImpl->depthRead.Get();
}


ID3D11RasterizerState* CommonStates::CullNone()
{
    if (!pImpl->cullNone)
    {
        pImpl->CreateRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID, &pImpl->cullNone);
    }

    return pImpl->cullNone.Get();
}


ID3D11RasterizerState* CommonStates::CullClockwise()
{
    if (!pImpl->cullClockwise)
    {
        pImpl->CreateRasterizerState(D3D11_CULL_FRONT, D3D11_FILL_SOLID, &pImpl->cullClockwise);
    }

    return pImpl->cullClockwise.Get();
}


ID3D11RasterizerState* CommonStates::CullCounterClockwise()
{
    if (!pImpl->cullCounterClockwise)
    {
        pImpl->CreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID, &pImpl->cullCounterClockwise);
    }

    return pImpl->cullCounterClockwise.Get();
}


ID3D11RasterizerState* CommonStates::Wireframe()
{
    if (!pImpl->wireframe)
    {
        pImpl->CreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_WIREFRAME, &pImpl->wireframe);
    }

    return pImpl->wireframe.Get();
}


ID3D11SamplerState* CommonStates::PointWrap()
{
    if (!pImpl->pointWrap)
    {
        pImpl->CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, &pImpl->pointWrap);
    }

    return pImpl->pointWrap.Get();
}


ID3D11SamplerState* CommonStates::PointClamp()
{
    if (!pImpl->pointClamp)
    {
        pImpl->CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, &pImpl->pointClamp);
    }

    return pImpl->pointClamp.Get();
}


ID3D11SamplerState* CommonStates::LinearWrap()
{
    if (!pImpl->linearWrap)
    {
        pImpl->CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, &pImpl->linearWrap);
    }

    return pImpl->linearWrap.Get();
}


ID3D11SamplerState* CommonStates::LinearClamp()
{
    if (!pImpl->linearClamp)
    {
        pImpl->CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, &pImpl->linearClamp);
    }

    return pImpl->linearClamp.Get();
}


ID3D11SamplerState* CommonStates::AnisotropicWrap()
{
    if (!pImpl->anisotropicWrap)
    {
        pImpl->CreateSamplerState(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, &pImpl->anisotropicWrap);
    }

    return pImpl->anisotropicWrap.Get();
}


ID3D11SamplerState* CommonStates::AnisotropicClamp()
{
    if (!pImpl->anisotropicClamp)
    {
        pImpl->CreateSamplerState(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_CLAMP, &pImpl->anisotropicClamp);
    }

    return pImpl->anisotropicClamp.Get();
}
