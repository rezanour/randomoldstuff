#pragma once

#include <Platform.h>
#include <GraphicsDevice.h>

namespace GDK
{
    class Effect;

    // Base implementation of the graphics device for all device types (DirectX, OpenGL, etc...)
    class GraphicsDeviceBase : public GraphicsDevice
    {
    public:
        static std::shared_ptr<GraphicsDeviceBase> Create(_In_ const GraphicsDevice::CreateParameters& parameters);

        // Configure Rendering Info
        virtual void ClearBoundResources() override;
        virtual void BindTexture(_In_ uint32_t slot, _In_ const std::shared_ptr<Texture>& texture) override;
        virtual void BindGeometry(_In_ const std::shared_ptr<Geometry>& geometry) override;
        virtual void SetViewProjection(_In_ const Matrix& view, _In_ const Matrix& projection) override;

        // Render the currently bound geometry
        virtual void Draw(_In_ const Matrix& world, _In_ uint32_t frame) override;

        // These are in pixels. The Draw2D method is relative to the workArea
        virtual void Set2DWorkArea(_In_ uint32_t width, _In_ uint32_t height) override;
        virtual void Draw2D(_In_ const std::shared_ptr<Texture>& texture, _In_ int32_t x, _In_ int32_t y) override;

        // Access for effects and other internal graphics types
        const Matrix& GetViewMatrix() const;
        const Matrix& GetProjectionMatrix() const;
        const Vector2& GetWorkArea() const;
        const Vector2& GetViewArea() const;

        // Test Test
        virtual std::shared_ptr<Geometry> CreateDynamicGeometry(_In_ uint32_t numVertices) = 0;

    protected:
        GraphicsDeviceBase(_In_ const GraphicsDevice::CreateParameters& parameters);
        virtual ~GraphicsDeviceBase();

        virtual void InitializeDevice() = 0;
        virtual std::shared_ptr<Texture> CreateDefaultTexture(_In_ uint32_t width, _In_ uint32_t height, _In_ TextureFormat format, _In_ const byte_t* pixels) = 0;
        virtual std::shared_ptr<Geometry> CreateQuad2D(_In_ uint32_t numVertices, _In_ const Geometry::Vertex2D* vertices, _In_ uint32_t numIndices, _In_ const uint32_t* indices) = 0;

    private:
        // Some of the initialization requires being able to pass a shared pointer to ourself around.
        // In order to do that, we need to have created at least 1 shared pointer beforehand, so this
        // can't happen during construction. This two-phase initialization allows us to do the right thing.
        void FinishInitialization();

        Matrix _view, _projection;
        Vector2 _workArea, _viewArea;

        static const uint32_t MaxTextures = 3;
        std::shared_ptr<Texture> _boundTextures[MaxTextures];
        std::shared_ptr<Texture> _defaultTexture;
        std::shared_ptr<Geometry> _boundGeometry;
        std::shared_ptr<Geometry> _quad2DGeometry;

        Geometry::Vertex2D _quadVertices[4];

        std::shared_ptr<Effect> _currentEffect;
        std::shared_ptr<Effect> _draw2DEffect;
        std::shared_ptr<Effect> _static3DEffect;
    };
}
