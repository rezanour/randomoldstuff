#pragma once

#include <stde\types.h>
#include <GDK\RefCounted.h>

#define DIRECTXMATH_INTEROP
#include <Lucid3D.Math.h>

#include "GraphicsTypes.h"
#include "BaseShader.h"

#include <vector>

namespace Lucid3D
{
    class LightCombineShader : public BaseShader
    {
    public:
        LightCombineShader();
        ~LightCombineShader();

        GDK_IMETHOD Initialize(_In_ Renderer* pRenderer);

        GDK_IMETHOD RenderBatch(_In_ ContextPtr& spContext, _In_ CameraPtr& spCamera, _In_ std::vector<std::shared_ptr<RenderTask>>& renderTasks);

    private:
        void SetupPerFrame(_In_ ContextPtr& spContext, _In_ CameraPtr& spCamera);

        // TODO: Find a way to share this with other shaders that need to render quads,
        // such as point lights, full screen effects, etc...
        stde::com_ptr<ID3D11VertexShader> _spQuadVertexShader;
        stde::com_ptr<ID3D11PixelShader> _spPixelShader;

        struct FrameConstants
        {
            Lucid3D::Matrix InvViewProjMatrix;
            Lucid3D::Vector3 CameraPosition;
            float _padding_;
            Lucid3D::Vector2 ScreenSize;
            Lucid3D::Vector2 _padding2_;
        };

        FrameConstants _frameConstants;
        BufferPtr _spFrameConstantBuffer;
    };
}
