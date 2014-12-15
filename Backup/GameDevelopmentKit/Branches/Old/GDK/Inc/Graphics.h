#pragma once

namespace GDK {
namespace Graphics {

    // Forward declarations
    struct IGraphicsDeviceFactory;

    struct IGraphicsDevice;
    struct IGraphicsResource;
    struct IRuntimeGeometry;
    struct IRuntimeTexture;

    enum class GraphicsDeviceScaleMode
    {
        FullScreen,
        // don't resize backbuffer, just scale output to fit window
        ScaleToFit,
        // resize the backbuffer to match 1:1 with the output
        ResizeToFit,
    };

    struct GraphicsDeviceCreationParameters
    {
        void* windowIdentity;
        size_t backBufferWidth;
        size_t backBufferHeight;
        bool vsyncEnabled;
        GraphicsDeviceScaleMode scaleMode;
    };

    //
    // GraphicsDeviceFactory is the component factory for creating GraphicsDevices. 
    //

    GDKINTERFACE IGraphicsDeviceFactory : public IUnknown
    {
        IMPL_GUID(0xafd76c6a, 0xf97c, 0x4768, 0x82, 0x92, 0xf1, 0x54, 0x5, 0xcd, 0x4b, 0xd4);

        virtual HRESULT GDKAPI CreateGraphicsDevice(_In_ const GraphicsDeviceCreationParameters& parameters, _COM_Outptr_ IGraphicsDevice** device)= 0;
    };

    //
    // GraphicsDevice is a cross platform abstraction over the GPU.
    //
    // It provides 2 main pieces of functionality:
    //      1. Control of the GPU, including managing the entire pipeline from input to output
    //      2. Creating and managing GPU resources
    //

    enum class GraphicsFillMode
    {
        Solid = 0,
        Wireframe
    };

    GDKINTERFACE IGraphicsDevice : public IUnknown
    {
        IMPL_GUID(0x5f2b1065, 0xed65, 0x4d51, 0xbd, 0xe5, 0xda, 0xe3, 0x58, 0x33, 0xaf, 0x5b);

        // Basic device control
        virtual void GDKAPI Clear(_In_ const Vector4& color) = 0;
        virtual void GDKAPI ClearDepth(_In_ float depth) = 0;
        virtual void GDKAPI Present() = 0;

        virtual void GDKAPI SetFillMode(_In_ GraphicsFillMode fillMode) = 0;

        // GPU resources
        virtual HRESULT GDKAPI CreateRuntimeGeometry(_In_ GDK::Content::IGeometryResource* data, _COM_Outptr_ IRuntimeGeometry** geometry) = 0;
        virtual HRESULT GDKAPI CreateRuntimeTexture(_In_ GDK::Content::ITextureResource* data, _COM_Outptr_ IRuntimeTexture** texture) = 0;

        // Configure Rendering Info
        virtual HRESULT GDKAPI ClearBoundResources() = 0;
        virtual HRESULT GDKAPI BindTexture(_In_ size_t slot, _In_opt_ IRuntimeTexture* texture) = 0;
        virtual HRESULT GDKAPI BindGeometry(_In_opt_ IRuntimeGeometry* geometry) = 0;
        virtual HRESULT GDKAPI SetViewProjection(_In_ const Matrix& view, _In_ const Matrix& projection) = 0;

        // Render the currently bound geometry
        virtual HRESULT GDKAPI Draw(_In_ const Matrix& world) = 0;

        // Render texture 0 as a screen space quad (normalized x, y from 0 -> 1)
        virtual HRESULT GDKAPI Draw2D(_In_ const RECT& destRect) = 0;
    };

    //
    // GraphicsResource is the base of all GPU based resources.
    //

    GDKINTERFACE IGraphicsResource : public IUnknown
    {
        IMPL_GUID(0xd8ff5558, 0xca4e, 0x46e1, 0x8d, 0xbc, 0xd8, 0x4f, 0x62, 0x48, 0xce, 0x23);

        // Is the resource currently bound to the pipeline
        virtual bool GDKAPI IsBound() const = 0;
        virtual const wchar_t* GDKAPI GetName() const = 0;
    };

    //
    // Runtime resources
    //

    GDKINTERFACE IRuntimeGeometry : public IGraphicsResource
    {
        IMPL_GUID(0xaff16aa3, 0x6aa1, 0x4587, 0x90, 0x9c, 0x96, 0xd7, 0x10, 0xfb, 0x28, 0xab);

        virtual size_t GDKAPI GetNumFrames() const = 0;
        virtual void GDKAPI SetCurrentFrame(_In_ size_t frame) = 0;
    };

    GDKINTERFACE IRuntimeTexture : public IGraphicsResource
    {
        IMPL_GUID(0x31e3ff9e, 0x9026, 0x4b7f, 0xa5, 0xe6, 0x9, 0xcd, 0xfd, 0x64, 0x31, 0x6d);
    };

} // Graphics
} // GDK
