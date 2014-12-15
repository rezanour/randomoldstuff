#pragma once

#include <GDK\Shaders.h>

#include "GraphicsTypes.h"
#include "ResourceBase.h"
#include <map>

#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Lucid3D
{
    class Renderer;
    class BaseShader;

    class Geometry;
    typedef stde::ref_counted_ptr<Geometry> GeometryPtr;

    class Geometry : public ResourceBase<Geometry>
    {
    public:
        static HRESULT CreateFullscreenQuad(_In_ Renderer* pRenderer, _In_ IContentManagerPtr& spContentManager, _Inout_ GeometryPtr& spGeometry);

        Geometry(_In_ Renderer* pRenderer, _In_ IContentManagerPtr& spContentManager, _In_ uint64 contentId);

        // Methods
        HRESULT GenerateInputLayout(_In_ stde::ref_counted_ptr<BaseShader>& spShader);
        HRESULT Draw(_In_ uint32 shaderId, _In_ ContextPtr& spContext);

    protected:
        GDK_METHOD OnLoad(_In_ Renderer* pRenderer, _In_ IContentManagerPtr spContentManager, _In_ uint64 contentId);
        GDK_METHOD OnUnload();

    private:
        VertexBufferPtr _spVertexBuffer;
        IndexBufferPtr _spIndexBuffer;

        typedef std::map<uint32, InputLayoutPtr> InputLayoutMap;
        InputLayoutMap _inputLayouts;

        typedef std::map<GDK::ShaderParameter::SemanticEnum, uint32> OffsetMap;
        OffsetMap _vertexChannels;
    };
}

