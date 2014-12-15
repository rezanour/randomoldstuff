#include "Precomp.h"

#if defined(WIN32)
#   include "OpenGLRendererWinClassic\OpenGLGraphicsDeviceWindows.h"
#elif defined(__APPLE__)
#   include "OpenGLGraphicsDeviceOSX.h"
#endif

static const char *g_staticGeometryVertexShaderSource =
    "uniform mat4 World;\n"
    "uniform mat4 InvTransWorld;\n"
    "uniform mat4 ViewProjection;\n"
    "\n"
    "attribute vec3 inPosition;\n"
    "attribute vec3 inNormal;\n"
    "attribute vec2 inUV;\n"
    "\n"
    "varying vec3 WorldNormal;\n"
    "varying vec2 UV;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   gl_Position = ViewProjection * World * vec4(inPosition, 1.0);\n"
    "   WorldNormal = (InvTransWorld * vec4(inNormal, 1.0)).xyz;\n"
    "   UV = inUV;\n"
    "}\n";

static const char *g_texturedFragmentShaderSource =
    "struct DirectionalLight\n"
    "{\n"
    "   vec3 Direction;\n"
    "   vec4 Color;\n"
    "};\n"
    "\n"
    "uniform DirectionalLight Light1;\n"
    "uniform DirectionalLight Light2;\n"
    "\n"
    "uniform sampler2D Sampler;\n"
    "\n"
    "varying vec3 WorldNormal;\n"
    "varying vec2 UV;\n"
    "\n"
    "vec4 ComputeDirectionalLight(vec3 normal, DirectionalLight light)\n"
    "{\n"
    "   float NdL = dot(normal, light.Direction);\n"
    "   return light.Color * clamp(NdL, 0.0, 1.0);\n"
    "}\n"
    "\n"
    "void main()\n"
    "{\n"
    "   vec4 samp = texture2D(Sampler, UV);\n"
    "   vec3 n = normalize(WorldNormal);\n"
    "   gl_FragColor = samp * (ComputeDirectionalLight(n, Light1) + ComputeDirectionalLight(n, Light2));\n"
    "}\n";

using Microsoft::WRL::ComPtr;

namespace GDK {
namespace Graphics {

#if defined(__APPLE__)
IGraphicsDevice* CreateGraphicsDeviceForOSX(const GraphicsDeviceCreationParameters& parameters)
{
    return OpenGLGraphicsDevice::Create(parameters).Detach();
}
#endif

const wchar_t* const OpenGLGraphicsDevice::DisplayName = L"OpenGL Renderer";

// IGraphicsDeviceFactory
HRESULT GDKAPI OpenGLGraphicsDeviceFactory::CreateGraphicsDevice(_In_ const GraphicsDeviceCreationParameters& parameters, _COM_Outptr_ IGraphicsDevice** device)
{
    MODULE_GUARD_BEGIN

    *device = static_cast<IGraphicsDevice*>(OpenGLGraphicsDevice::Create(parameters).Get());
    if (*device)
    {
        (*device)->AddRef();
    }

    MODULE_GUARD_END
}

OpenGLGraphicsDevice::OpenGLGraphicsDevice(_In_ const GraphicsDeviceCreationParameters& parameters) :
    _parameters(parameters)
{
}

ComPtr<OpenGLGraphicsDevice> OpenGLGraphicsDevice::Create(_In_ const GraphicsDeviceCreationParameters& parameters)
{
    ComPtr<OpenGLGraphicsDevice> device;

#if defined(WIN32)
    device = Make<OpenGLGraphicsDeviceWindows>(parameters);
#elif defined(__APPLE__)
    device = Make<OpenGLGraphicsDeviceOSX>(parameters);
#endif

    //
    // Compile shaders
    //

    CompileShader(g_staticGeometryVertexShaderSource, GL_VERTEX_SHADER, &device->_vertexShader);
    CompileShader(g_texturedFragmentShaderSource, GL_FRAGMENT_SHADER, &device->_fragmentShader);

    // Combine the shaders into a program, and link it
    CHECK_GL(device->_shaderProgram = glCreateProgram());
    CHECK_GL(glAttachShader(device->_shaderProgram, device->_vertexShader));
    CHECK_GL(glAttachShader(device->_shaderProgram, device->_fragmentShader));

    CHECK_GL(glBindAttribLocation(device->_shaderProgram, 0, "inPosition"));
    CHECK_GL(glBindAttribLocation(device->_shaderProgram, 1, "inNormal"));
    CHECK_GL(glBindAttribLocation(device->_shaderProgram, 2, "inUV"));

    CHECK_GL(glLinkProgram(device->_shaderProgram));

    GLchar log[500] = {0};
    GLsizei length = 0;
    glGetProgramInfoLog(device->_shaderProgram, _countof(log), &length, log);

#ifdef WIN32
    DebugOut(L"Program linking results:\n %S\n", log);
#else
    DebugOut(L"Program linking results:\n %s\n", log);
#endif


    // Set it on the device as our active shader
    CHECK_GL(glUseProgram(device->_shaderProgram));

    // Get the constant addresses
    CHECK_GL(device->_vsConstants.World = glGetUniformLocation(device->_shaderProgram, "World"));
    CHECK_GL(device->_vsConstants.InvTransWorld = glGetUniformLocation(device->_shaderProgram, "InvTransWorld"));
    CHECK_GL(device->_vsConstants.ViewProjection = glGetUniformLocation(device->_shaderProgram, "ViewProjection"));
    CHECK_GL(device->_psConstants.Light1.Direction = glGetUniformLocation(device->_shaderProgram, "Light1.Direction"));
    CHECK_GL(device->_psConstants.Light1.Color = glGetUniformLocation(device->_shaderProgram, "Light1.Color"));
    CHECK_GL(device->_psConstants.Light2.Direction = glGetUniformLocation(device->_shaderProgram, "Light2.Direction"));
    CHECK_GL(device->_psConstants.Light2.Color = glGetUniformLocation(device->_shaderProgram, "Light2.Color"));

    // Light 1
    Vector3 direction(0, 0, -1);
    Vector4 color(1, 1, 1, 1);
    CHECK_GL(glUniform3fv(device->_psConstants.Light1.Direction, 1, reinterpret_cast<float*>(&direction)));
    CHECK_GL(glUniform4fv(device->_psConstants.Light1.Color, 1, reinterpret_cast<float*>(&color)));

    // Light 2
    direction = Vector3(0, 1, 0);
    color = Vector4(0, 0, 1, 1);
    CHECK_GL(glUniform3fv(device->_psConstants.Light2.Direction, 1, reinterpret_cast<float*>(&direction)));
    CHECK_GL(glUniform4fv(device->_psConstants.Light2.Color, 1, reinterpret_cast<float*>(&color)));

    // Let GL know that our triangles are clockwise winding order
    CHECK_GL(glFrontFace(GL_CW));

    //
    // texture sampler
    // NICK: Samplers are only available in OpenGL 3.3 or higher: http://www.opengl.org/sdk/docs/man3/xhtml/glBindSampler.xml
    //
    //glGenSamplers(1, &device->_sampler);
    //glSamplerParameteri(device->_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glSamplerParameteri(device->_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Default texture
    byte_t lightGreyPixel[] = { 190, 190, 190, 255 };
    device->_defaultTexture = RuntimeTexture::CreateFromData(1, 1, lightGreyPixel);

    // Quad
    Vector3 positions[] =
    {
        Vector3(-1.0f, 1.0f, 0.1f),
        Vector3(1.0f, 1.0f, 0.1f),
        Vector3(1.0f, -1.0f, 0.1f),
        Vector3(1.0f, -1.0f, 0.1f),
        Vector3(-1.0f, -1.0f, 0.1f),
        Vector3(-1.0f, 1.0f, 0.1f),
    };

    Vector3 normals[] = 
    {
        Vector3(0.0f, 0.0f, -1.0f),
        Vector3(0.0f, 0.0f, -1.0f),
        Vector3(0.0f, 0.0f, -1.0f),
        Vector3(0.0f, 0.0f, -1.0f),
        Vector3(0.0f, 0.0f, -1.0f),
        Vector3(0.0f, 0.0f, -1.0f),
    };

    Vector2 uvs[] =
    {
        Vector2(0.0f, 0.0f),
        Vector2(1.0f, 0.0f),
        Vector2(1.0f, 1.0f),
        Vector2(1.0f, 1.0f),
        Vector2(0.0f, 1.0f),
        Vector2(0.0f, 0.0f),
    };

    uint32_t notused[] = { 0 };

    device->_quad = RuntimeGeometry::CreateFromData(_countof(positions), positions, normals, uvs, 0, notused);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    return device.Detach();
}

void GDKAPI OpenGLGraphicsDevice::CompileShader(_In_z_ const char* shaderSource, _In_ GLenum shaderType, _Out_ UINT* shader)
{
    *shader = glCreateShader(shaderType);
    glShaderSource(*shader, 1, &shaderSource, nullptr);
    glCompileShader(*shader);

    int status;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);

    GLchar log[500] = {0};
    GLsizei length = 0;
    glGetShaderInfoLog(*shader, _countof(log), &length, log);

#if WIN32
    DebugOut(L"Shader compilation results:\n %S\n", log);
#else
    DebugOut(L"Shader compilation results:\n %s\n", log);
#endif

    CHECK_IS_TRUE(status == GL_TRUE, E_UNEXPECTED);
}


// IGraphicsDevice
void GDKAPI OpenGLGraphicsDevice::Clear(_In_ const Vector4& color)
{
    MakeContextCurrent();

    glClearColor(color.x, color.y, color.z, color.w);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GDKAPI OpenGLGraphicsDevice::ClearDepth(_In_ float depth)
{
    MakeContextCurrent();

    glClearDepth(depth);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void GDKAPI OpenGLGraphicsDevice::SetFillMode(_In_ GraphicsFillMode fillMode)
{
    UNREFERENCED_PARAMETER(fillMode);
}


HRESULT GDKAPI OpenGLGraphicsDevice::CreateRuntimeGeometry(_In_ GDK::Content::IGeometryResource* data, _COM_Outptr_ IRuntimeGeometry** geometry)
{
    MODULE_GUARD_BEGIN

    CHECKHR(RuntimeGeometry::Create(data).CopyTo(geometry));

    MODULE_GUARD_END
}

HRESULT GDKAPI OpenGLGraphicsDevice::CreateRuntimeTexture(_In_ GDK::Content::ITextureResource* data, _COM_Outptr_ IRuntimeTexture** texture)
{
    MODULE_GUARD_BEGIN

    CHECKHR(RuntimeTexture::Create(data).CopyTo(texture));

    MODULE_GUARD_END
}

HRESULT GDKAPI OpenGLGraphicsDevice::ClearBoundResources()
{
    if (_boundGeometry)
    {
        _boundGeometry->Unbind();
        _boundGeometry = nullptr;
    }

    if (_boundTexture)
    {
        _boundTexture->Unbind();
        _boundTexture = nullptr;
    }

    BindTexture(0, _defaultTexture.Get());

    return S_OK;
}

HRESULT GDKAPI OpenGLGraphicsDevice::BindTexture(_In_ size_t slot, _In_opt_ IRuntimeTexture* texture)
{
    if (_boundTexture)
    {
        _boundTexture->Unbind();
    }

    _boundTexture = reinterpret_cast<RuntimeTexture*>(texture);

    if (!_boundTexture)
    {
        _boundTexture = _defaultTexture;
    }

    if (_boundTexture)
    {
        _boundTexture->Bind();
        glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(slot));
        // NICK: Samplers are only available in OpenGL 3.3 or higher: http://www.opengl.org/sdk/docs/man3/xhtml/glBindSampler.xml
        // glBindSampler(static_cast<GLenum>(slot), _sampler);
    }

    return S_OK;
}

HRESULT GDKAPI OpenGLGraphicsDevice::BindGeometry(_In_opt_ IRuntimeGeometry* geometry)
{
    if (_boundGeometry)
    {
        _boundGeometry->Unbind();
    }

    _boundGeometry = reinterpret_cast<RuntimeGeometry*>(geometry);

    if (_boundGeometry)
    {
        _boundGeometry->Bind();
    }

    return S_OK;
}

HRESULT GDKAPI OpenGLGraphicsDevice::SetViewProjection(_In_ const Matrix& view, _In_ const Matrix& projection)
{
    Matrix vp = view * projection;
    glUniformMatrix4fv(_vsConstants.ViewProjection, 1, GL_FALSE, reinterpret_cast<float*>(&vp));
    return S_OK;
}

HRESULT GDKAPI OpenGLGraphicsDevice::Draw(_In_ const Matrix& world)
{
    glUniformMatrix4fv(_vsConstants.World, 1, GL_FALSE, reinterpret_cast<const float*>(&world));
    Matrix invTransWorld;
    Matrix::Inverse(world, &invTransWorld);
    // Use the bool to have the API transpose for us
    glUniformMatrix4fv(_vsConstants.InvTransWorld, 1, GL_TRUE, reinterpret_cast<const float*>(&invTransWorld));

    if (_boundGeometry)
    {
        _boundGeometry->Draw();
    }

    return S_OK;
}

HRESULT GDKAPI OpenGLGraphicsDevice::Draw2D(_In_ const RECT& destRect)
{
    Vector2 size(static_cast<float>(destRect.right - destRect.left) / 100.0f, static_cast<float>(destRect.bottom - destRect.top) / 100.0f);
    Vector2 position(destRect.left / 100.0f + size.x * 0.5f, destRect.top / 100.0f + size.y * 0.5f);

    Matrix world = Matrix::CreateTranslation(Vector3(position.x - 0.5f, position.y - 0.5f, 0.0f)) * Matrix::CreateScale(Vector3(size.x, size.y, 1.0f));

    auto previousGeometry = _boundGeometry;
    Matrix viewProj;
    glGetUniformfv(_shaderProgram, _vsConstants.ViewProjection, reinterpret_cast<float*>(&viewProj));

    SetViewProjection(Matrix::Identity(), Matrix::Identity());

    BindGeometry(_quad.Get());
    Draw(world);

    BindGeometry(previousGeometry.Get());

    glUniformMatrix4fv(_vsConstants.ViewProjection, 1, GL_FALSE, reinterpret_cast<float*>(&viewProj));

    return S_OK;
}

} // Graphics
} // GDK
