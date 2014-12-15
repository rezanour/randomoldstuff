#pragma once

#include <stde\types.h>
#include <GDK\RefCounted.h>

#define DIRECTXMATH_INTEROP
#include <L3DMath\L3DMath.h>

#include "GraphicsTypes.h"
#include "BaseShader.h"

#include <vector>

namespace Lucid3D
{
    class SimpleShader : public BaseShader
    {
    public:
        SimpleShader();
        ~SimpleShader();

        GDK_METHOD Initialize(_In_ Renderer* pRenderer);

        GDK_METHOD RenderBatch(_In_ ContextPtr& spContext, _In_ CameraPtr& spCamera, _In_ std::vector<std::shared_ptr<RenderTask>>& renderTasks);

    private:
        void SetupPerFrame(_In_ ContextPtr& spContext, _In_ CameraPtr& spCamera);

        stde::com_ptr<ID3D11VertexShader> _spVertexShader;
        stde::com_ptr<ID3D11PixelShader> _spPixelShader;
        stde::com_ptr<ID3D11SamplerState> _spLinearSampler;
        stde::com_ptr<ID3D11SamplerState> _spPointSampler;

        struct FrameConstants
        {
            L3DMath::Matrix ViewProjMatrix;
            L3DMath::Vector3 CameraPosition;
            float OneOverFarClipDistance;
        };

        FrameConstants _frameConstants;
        BufferPtr _spFrameConstantBuffer;

        struct ObjectConstants
        {
            L3DMath::Matrix WorldMatrix;
        };

        ObjectConstants _objectConstants;
        BufferPtr _spObjectConstantBuffer;
    };
}
