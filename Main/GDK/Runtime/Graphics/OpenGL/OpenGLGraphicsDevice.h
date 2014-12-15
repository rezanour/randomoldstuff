#pragma once

#include <GraphicsDevice.h>
#include <functional>
#include "GDKGL.h"

namespace GDK 
{
    class OpenGLGraphicsDevice : public GraphicsDevice
    {
    public:
        static std::shared_ptr<OpenGLGraphicsDevice> Create(_In_ const GraphicsDevice::CreateParameters& parameters);
        virtual ~OpenGLGraphicsDevice();
        
        // Basic device control
        virtual void Clear(_In_ const Vector4& color) override;
        virtual void ClearDepth(_In_ float depth) override;
        virtual void Present() = 0; // Present is platform specific

        // GPU resources
        virtual std::shared_ptr<Geometry> CreateGeometry(_In_ const std::shared_ptr<GeometryContent>& data) override;
        virtual std::shared_ptr<Texture> CreateTexture(_In_ const std::shared_ptr<TextureContent>& data) override;

        // Configure Rendering Info
        virtual void ClearBoundResources() override;
        virtual void BindTexture(_In_ uint32_t slot, _In_ const std::shared_ptr<Texture>& texture) override;
        virtual void BindGeometry(_In_ const std::shared_ptr<Geometry>& geometry) override;
        virtual void SetViewProjection(_In_ const Matrix& view, _In_ const Matrix& projection) override;

        // Render the currently bound geometry
        virtual void Draw(_In_ const Matrix& world, _In_ uint32_t frame) override;

        virtual void Set2DWorkArea(_In_ uint32_t width, _In_ uint32_t height) override { UNREFERENCED_PARAMETER(width); UNREFERENCED_PARAMETER(height); }
        virtual void Draw2D(_In_ const std::shared_ptr<Texture>& texture, _In_ int32_t x, _In_ int32_t y) override { UNREFERENCED_PARAMETER(texture); UNREFERENCED_PARAMETER(x); UNREFERENCED_PARAMETER(y); }

    protected:
        OpenGLGraphicsDevice(_In_ const GraphicsDevice::CreateParameters& parameters);
        virtual void MakeContextCurrent() = 0;

        void Initialize();

        // Destructor of subclasses needs to call this before destroying the GL context so the base
        // can properly clean up any GL resources.
        void CleanUpBase();

    private:
        void CreateDefaultTexture();
        void CreateQuad();
        void SetLight(_In_ uint32_t light, _In_ const Vector3& direction, _In_ const Vector4& color);
        
        GLuint _vertexShader;
        GLuint _fragmentShader;
        GLuint _program;
        
        GLuint _worldUniform;
        GLuint _viewProjUniform;
        GLuint _samplerUniform;
        
        // GLSL can't set whole structs; we have to set individual members so we use a struct to
        // hold the two uniforms for a single light and then an array of those structs to handle all 3 lights.
        struct LightUniform
        {
            LightUniform() : directionUniform(0), colorUniform(0) { }

            GLuint directionUniform;
            GLuint colorUniform;
        };
        std::array<LightUniform, 3> _lightUniforms;

        // Render Info
        std::shared_ptr<Geometry> _boundGeometry;
        std::array<std::shared_ptr<Texture>, 2> _boundTextures;

        Matrix _viewProjection;
                
        // Default texture used for untextured geometries
        std::shared_ptr<Texture> _defaultTexture;

        // Quad used for 2D rendering
        std::shared_ptr<Geometry> _quad;
    };
} 