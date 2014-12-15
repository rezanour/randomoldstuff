#pragma once

#include "Platform.h"

namespace GDK
{
    class GeometryContent;
    class TextureContent;

    class Geometry;
    class Texture;

    // TODO: Move this out to a more general header, not specific to graphics
    struct RectangleF
    {
        float Left;
        float Top;
        float Width;
        float Height;
    };

    class GraphicsDevice : public RuntimeObject<GraphicsDevice>
    {
    public:
        enum class Type
        {
            OpenGL,
#ifdef WIN32
            DirectX11,
#endif
        };

        struct CreateParameters
        {
            CreateParameters()
            {
            }

            CreateParameters(_In_ Type type, _In_ void* identity, _In_ uint32_t width, _In_ uint32_t height, _In_ bool vsync)
            {
            }

            Type Type;
            void* WindowIdentity;
            uint32_t BackBufferWidth;
            uint32_t BackBufferHeight;
            bool VSyncEnabled;
        };

        // Enumeration and creation support
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
        virtual void BindGeometry(_In_ const std::shared_ptr<Geometry>& geometry) = 0;
        virtual void BindTexture(_In_ size_t slot, _In_ const std::shared_ptr<Texture>& texture) = 0;
        virtual void SetViewProjection(_In_ const Matrix& view, _In_ const Matrix& projection) = 0;

        // Render the currently bound geometry
        virtual void Draw(_In_ const Matrix& world) = 0;

        // Render texture as a screen space quad (normalized x, y from 0 -> 1)
        virtual void Draw2D(_In_ size_t slot, _In_ const RectangleF& destRect) = 0;

    protected:
        GraphicsDevice(_In_ const GraphicsDeviceCreationParameters& parameters);

        const GraphicsDeviceCreationParameters _parameters;
    };
}
