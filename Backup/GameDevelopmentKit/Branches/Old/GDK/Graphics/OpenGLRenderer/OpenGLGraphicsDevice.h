#pragma once

namespace GDK {
namespace Graphics {

    class RuntimeGeometry;
    class RuntimeTexture;

    class OpenGLGraphicsDeviceFactory : public RuntimeObject<IGraphicsDeviceFactory>
    {
        IMPL_GUID(0xaf47b3dd, 0xb34c, 0x4673, 0xac, 0xaa, 0xfa, 0xc, 0x54, 0x62, 0x94, 0x5c);
    public:
        static Microsoft::WRL::ComPtr<OpenGLGraphicsDeviceFactory> Create();

        // IGraphicsDeviceFactory
        virtual HRESULT GDKAPI CreateGraphicsDevice(_In_ const GraphicsDeviceCreationParameters& parameters, _COM_Outptr_ IGraphicsDevice** device);
    };

    class OpenGLGraphicsDevice : public RuntimeObject<IGraphicsDevice>
    {
    public:
        static const wchar_t* const DisplayName;

        static Microsoft::WRL::ComPtr<OpenGLGraphicsDevice> Create(_In_ const GraphicsDeviceCreationParameters& parameters);
        
        //
        // IGraphicsDevice
        //

        // Basic device control
        virtual void GDKAPI Clear(_In_ const Vector4& color);
        virtual void GDKAPI ClearDepth(_In_ float depth);
        virtual void GDKAPI Present() = 0; // Present is platform specific

        virtual void GDKAPI SetFillMode(_In_ GraphicsFillMode fillMode);

        // GPU resources
        virtual HRESULT GDKAPI CreateRuntimeGeometry(_In_ GDK::Content::IGeometryResource* data, _COM_Outptr_ IRuntimeGeometry** geometry);
        virtual HRESULT GDKAPI CreateRuntimeTexture(_In_ GDK::Content::ITextureResource* data, _COM_Outptr_ IRuntimeTexture** texture);

        // Configure Rendering Info
        virtual HRESULT GDKAPI ClearBoundResources();
        virtual HRESULT GDKAPI BindTexture(_In_ size_t slot, _In_opt_ IRuntimeTexture* texture);
        virtual HRESULT GDKAPI BindGeometry(_In_opt_ IRuntimeGeometry* geometry);
        virtual HRESULT GDKAPI SetViewProjection(_In_ const Matrix& view, _In_ const Matrix& projection);

        // Render the currently bound geometry
        virtual HRESULT GDKAPI Draw(_In_ const Matrix& world);

        // Render texture 0 as a screen space quad (normalized x, y from 0 -> 1)
        virtual HRESULT GDKAPI Draw2D(_In_ const RECT& destRect);
        
    protected:
        OpenGLGraphicsDevice(_In_ const GraphicsDeviceCreationParameters& parameters);
        
        virtual void GDKAPI MakeContextCurrent() = 0;

        GraphicsDeviceCreationParameters _parameters;

    private:
        // Compile a shader
        static void GDKAPI CompileShader(_In_z_ const char* shaderSource, _In_ GLenum shaderType, _Out_ UINT* shader);

        // Shaders
        UINT _vertexShader;
        UINT _fragmentShader;
        UINT _shaderProgram;
        UINT _sampler;

        // Constants
        struct VSConstants
        {
            int World;
            int InvTransWorld;
            int ViewProjection;
        } _vsConstants;

        struct DirectionalLight
        {
            int Direction;
            int Color;
        };

        struct PSConstants
        {
            DirectionalLight Light1;
            DirectionalLight Light2;
        } _psConstants;

        // Render info
        Microsoft::WRL::ComPtr<RuntimeGeometry> _boundGeometry;
        Microsoft::WRL::ComPtr<RuntimeTexture> _boundTexture;
        Microsoft::WRL::ComPtr<RuntimeGeometry> _quad;
        Microsoft::WRL::ComPtr<RuntimeTexture> _defaultTexture;
    };

} // Graphics
} // GDK
