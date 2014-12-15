// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

#pragma once

#include <d3d11.h>
#include <memory>


namespace DirectX
{
    class CommonStates
    {
    public:
        explicit CommonStates(_In_ ID3D11Device* device);
        CommonStates(CommonStates&& moveFrom);
        CommonStates& operator= (CommonStates&& moveFrom);
        virtual ~CommonStates();

        // Blend states.
        ID3D11BlendState* Opaque();
        ID3D11BlendState* AlphaBlend();
        ID3D11BlendState* Additive();
        ID3D11BlendState* NonPremultiplied();

        // Depth stencil states.
        ID3D11DepthStencilState* DepthNone();
        ID3D11DepthStencilState* DepthDefault();
        ID3D11DepthStencilState* DepthRead();

        // Rasterizer states.
        ID3D11RasterizerState* CullNone();
        ID3D11RasterizerState* CullClockwise();
        ID3D11RasterizerState* CullCounterClockwise();
        ID3D11RasterizerState* Wireframe();

        // Sampler states.
        ID3D11SamplerState* PointWrap();
        ID3D11SamplerState* PointClamp();
        ID3D11SamplerState* LinearWrap();
        ID3D11SamplerState* LinearClamp();
        ID3D11SamplerState* AnisotropicWrap();
        ID3D11SamplerState* AnisotropicClamp();

    private:
        // Private implementation.
        class Impl;

        std::shared_ptr<Impl> pImpl;

        // Prevent copying.
        CommonStates(CommonStates const&);
        CommonStates& operator= (CommonStates const&);
    };
}
