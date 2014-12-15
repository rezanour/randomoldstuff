#pragma once

enum class VertexFormat;
enum class LightType;
struct ITexture;
struct IGeometry;
struct IModel;
struct ILight;
struct IGraphicsScene;
struct IBsp;

//===============================================================
// Core graphics system functionality
//
// Top level control of the entire graphics system. Manages the
// system configuration, and final presentation to the window.
// Provides access to 2D and 3D renderers, and creation of
// graphics resources.
//
//===============================================================

// Configuration data for starting up the graphics system.
struct GraphicsSystemConfig
{
    HWND Window;
    uint32_t Width;
    uint32_t Height;
    bool VSync;
};

struct __declspec(novtable) IGraphicsSystem
{
    virtual const GraphicsSystemConfig& GetConfig() const = 0;

    // Creates a surface which can be used as an input to rendering
    virtual std::shared_ptr<ITexture> CreateTexture(_In_ uint32_t width, _In_ uint32_t height, _In_ DXGI_FORMAT format, _In_opt_ const void* pixels, _In_ uint32_t pitch, _In_ bool generateMips) = 0;

    // Creates a geometry object from the specified vertices and indices
    virtual std::shared_ptr<IGeometry> CreateGeometry(_In_ uint32_t numVertices, _In_ VertexFormat format, _In_ const void* vertices, _In_ uint32_t numIndices, _In_ const uint32_t* indices) = 0;

    // Creates a model object which can be used for rendering
    virtual std::shared_ptr<IModel> CreateModel(_In_opt_ const std::shared_ptr<IReadonlyObject>& owner) = 0;

    // Creates a light source which can be used in the scene
    virtual std::shared_ptr<ILight> CreateLight(_In_ LightType type, _In_ const XMFLOAT3& color, _In_ float radius) = 0;

    // Creates a 3D scene, which can be rendered
    virtual std::shared_ptr<IGraphicsScene> CreateScene() = 0;

    // Render a 3D scene
    virtual void DrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection) = 0;

    // Draw 2D content (always on top of whatever 3D scene is being rendered).
    virtual void DrawImage(_In_ const std::shared_ptr<ITexture>& texture, _In_ int32_t x, _In_ int32_t y) = 0;
    virtual void DrawImage(_In_ const std::shared_ptr<ITexture>& texture, _In_ int32_t x, _In_ int32_t y, _In_ uint32_t width, _In_ uint32_t height) = 0;
    virtual void DrawImage(_In_ const std::shared_ptr<ITexture>& texture, _In_ const RECT& source, _In_ const RECT& dest) = 0;

    // This must be called to commit any drawing done since the last frame, and present the contents.
    virtual void RenderFrame() = 0;

    //
    // Stereo rendering support for the Oculus Rift
    //
    virtual bool IsStereoRenderingSupported() const = 0;

    // NOTE: This will change the resolution to match the native size of the dedicated stereo rendering device/headset
    virtual void EnableStereoRendering(_In_ bool enable) = 0;
    virtual bool IsStereoRenderingEnabled() const = 0;

    // HACK HACK: Move to Input
    virtual XMVECTOR GetHeadOrientation() const = 0;
};

std::shared_ptr<IGraphicsSystem> CreateGraphicsSystem(_In_ const GraphicsSystemConfig& config);
