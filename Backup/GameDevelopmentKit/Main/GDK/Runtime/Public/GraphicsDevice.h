#pragma once

#include "Platform.h"
#include "RuntimeObject.h"
#include "GDKMath.h"

#include "GeometryContent.h"
#include "TextureContent.h"

#include "Geometry.h"
#include "Texture.h"

namespace GDK
{
    class TextureContent;
    class GeometryContent;
    class Texture;
    class Geometry;

    class GraphicsDevice : public RuntimeObject<GraphicsDevice>
    {
    public:
        enum class Type
        {
            OpenGL,
#ifdef WIN32
            DirectX
#endif
        };

        struct CreateParameters
        {
            CreateParameters() : type(Type::OpenGL), windowIdentity(nullptr), backBufferWidth(0), backBufferHeight(0)
            {
            }

            CreateParameters(_In_ Type type, _In_ void* windowIdentity, _In_ uint32_t backBufferWidth, _In_ uint32_t backBufferHeight) :
                type(type), windowIdentity(windowIdentity), backBufferWidth(backBufferWidth), backBufferHeight(backBufferHeight)
            {
            }

            Type type;
            void* windowIdentity;
            uint32_t backBufferWidth;
            uint32_t backBufferHeight;
        };

        static std::shared_ptr<GraphicsDevice> Create(_In_ const CreateParameters& parameters);

        // Basic device control
        virtual void Clear(_In_ const Vector4& color) = 0;
        virtual void ClearDepth(_In_ float depth) = 0;
        virtual void Present() = 0;

        // GPU resources
        virtual std::shared_ptr<Geometry> CreateGeometry(_In_ const std::shared_ptr<GeometryContent>& data) = 0;
        virtual std::shared_ptr<Texture> CreateTexture(_In_ const std::shared_ptr<TextureContent>& data) = 0;

        // Configure Rendering Info
        virtual void ClearBoundResources() = 0;
        virtual void BindTexture(_In_ uint32_t slot, _In_ const std::shared_ptr<Texture>& texture) = 0;
        virtual void BindGeometry(_In_ const std::shared_ptr<Geometry>& geometry) = 0;
        virtual void SetViewProjection(_In_ const Matrix& view, _In_ const Matrix& projection) = 0;

        // Render the currently bound geometry
        virtual void Draw(_In_ const Matrix& world) = 0;

        // Render texture 0 as a screen space quad (normalized x, y from 0 -> 1)
        virtual void Draw2D(_In_ const RectangleF& destRect) = 0;

    protected:
        GraphicsDevice(_In_ const CreateParameters& parameters);

        const CreateParameters _parameters;
    };
}
