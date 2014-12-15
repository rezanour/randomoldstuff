#ifndef WIN32
#include "OpenGLGraphicsDevice.h"
#include <GDKError.h>
#include <FileSystem.h>
#include <FormatString.h>
#include <Log.h>
#include <iostream>

#if defined(WIN32)
#include "OpenGLGraphicsDeviceWindows.h"
#elif defined(__APPLE__)
#include "OpenGLGraphicsDeviceOSX.h"
#endif

#include "GlGeometry.h"
#include "GlTexture.h"

// Helper for getting uniform locations that not only does CHECK_GL but also checks that the returned location does not equal -1, the
// specified return value if a uniform location cannot be found.
#define GET_UNIFORM_LOCATION(var, program, name) CHECK_GL(var = glGetUniformLocation(program, name)); CHECK_TRUE(var != -1);

using namespace GDK;

static GLuint CompileShader(_In_ const char* shaderSource, _In_ GLenum shaderType);

_Use_decl_annotations_
    std::shared_ptr<OpenGLGraphicsDevice> OpenGLGraphicsDevice::Create(const GraphicsDevice::CreateParameters& parameters)
{
    std::shared_ptr<OpenGLGraphicsDevice> device;
#if defined(WIN32)
    device = std::shared_ptr<OpenGLGraphicsDevice>(GDKNEW OpenGLGraphicsDeviceWindows(parameters));
#elif defined(__APPLE__)
    device = std::shared_ptr<OpenGLGraphicsDevice>(GDKNEW OpenGLGraphicsDeviceOSX(parameters));
#endif

    device->MakeContextCurrent();
    CHECK_GL(const char* version = (const char*)glGetString(GL_VERSION));
    CHECK_GL(const char* vendor = (const char*)glGetString(GL_VENDOR));
    GDK::Log::WriteInfo(GDK::FormatString(L"OpenGL version: {0}, vendor: {1}\n", version, vendor));

    device->Initialize();

    return device;
}

_Use_decl_annotations_
    OpenGLGraphicsDevice::OpenGLGraphicsDevice(const GraphicsDevice::CreateParameters& parameters) : 
    GraphicsDevice(parameters),
    _vertexShader(0),
    _fragmentShader(0),
    _program(0),
    _worldUniform(0),
    _viewProjUniform(0),
    _samplerUniform(0)
{
}

OpenGLGraphicsDevice::~OpenGLGraphicsDevice()
{
}

_Use_decl_annotations_
    void OpenGLGraphicsDevice::Clear(const Vector4& color)
{
    MakeContextCurrent();

    glClearColor(color.x, color.y, color.z, color.w);
    glClear(GL_COLOR_BUFFER_BIT);
}

_Use_decl_annotations_
    void OpenGLGraphicsDevice::ClearDepth(float depth)
{
    MakeContextCurrent();

    glClearDepth(depth);
    glClear(GL_DEPTH_BUFFER_BIT);
}

_Use_decl_annotations_
    std::shared_ptr<Geometry> OpenGLGraphicsDevice::CreateGeometry(const std::shared_ptr<GeometryContent>& data)
{
    return GlGeometry::Create(data);
}

_Use_decl_annotations_
    std::shared_ptr<Texture> OpenGLGraphicsDevice::CreateTexture(const std::shared_ptr<TextureContent>& data)
{
    return GlTexture::Create(data);
}

void OpenGLGraphicsDevice::ClearBoundResources()
{
    BindGeometry(nullptr);

    for (uint32_t i = 0; i < _boundTextures.size(); ++i)
    {
        BindTexture(i, nullptr);
    }
}

_Use_decl_annotations_
void OpenGLGraphicsDevice::BindTexture(uint32_t slot, const std::shared_ptr<Texture>& texture)
{
    if (_boundTextures[slot] != nullptr)
    {
        static_cast<RuntimeTexture*>(_boundTextures[slot].get())->Unbind(slot);
    }

    if (texture != nullptr)
    {
        _boundTextures[slot] = texture;
    }
    else
    {
        if (_defaultTexture == nullptr)
        {
            CreateDefaultTexture();
        }
        _boundTextures[slot] = _defaultTexture;
    }

    if (_boundTextures[slot] != nullptr)
    {
        static_cast<RuntimeTexture*>(_boundTextures[slot].get())->Bind(slot);
    }
}

_Use_decl_annotations_
void OpenGLGraphicsDevice::BindGeometry(const std::shared_ptr<Geometry>& geometry)
{
    if (_boundGeometry != nullptr)
    {
        static_cast<RuntimeGeometry*>(_boundGeometry.get())->Unbind();
    }

    _boundGeometry = geometry;

    if (_boundGeometry != nullptr)
    {
        static_cast<RuntimeGeometry*>(_boundGeometry.get())->Bind();
    }
}

_Use_decl_annotations_
void OpenGLGraphicsDevice::SetViewProjection(const Matrix& view, const Matrix& projection)
{
    CHECK_GL(glUseProgram(_program));

    _viewProjection = view * projection;
    CHECK_GL(glUniformMatrix4fv(_viewProjUniform, 1, GL_FALSE, &_viewProjection.m00));

    CHECK_GL(glUseProgram(0));
}

_Use_decl_annotations_
void OpenGLGraphicsDevice::Draw(const Matrix& world, uint32_t frame)
{
    if (_boundGeometry)
    {
        CHECK_GL(glUseProgram(_program));
        CHECK_GL(glUniformMatrix4fv(_worldUniform, 1, GL_FALSE, &world.m00));

        static_cast<RuntimeGeometry*>(_boundGeometry.get())->Draw(frame);

        CHECK_GL(glUseProgram(0));
    }
}

_Use_decl_annotations_
void OpenGLGraphicsDevice::Draw2D(const RectangleF& destRect)
{
    CHECK_GL(glDisable(GL_DEPTH_TEST));
    
    Matrix world = 
        Matrix::CreateScale(Vector3(destRect.Width(), destRect.Height(), 1.0f)) *
        Matrix::CreateTranslation(Vector3(destRect.left - destRect.Width() * 0.5f, destRect.top - destRect.Height() * 0.5f, 0.0f));

    auto previousGeometry = _boundGeometry;
    auto previousViewProj = _viewProjection;

    SetViewProjection(Matrix::Identity(), Matrix::Identity());
    BindGeometry(_quad);
    Draw(world, 0);

    BindGeometry(previousGeometry);
    _viewProjection = previousViewProj;
    CHECK_GL(glUseProgram(_program));
    CHECK_GL(glUniformMatrix4fv(_viewProjUniform, 1, GL_FALSE, &_viewProjection.m00));
    CHECK_GL(glUseProgram(0));
    
    CHECK_GL(glEnable(GL_DEPTH_TEST));
}

void OpenGLGraphicsDevice::Initialize()
{
    // Open streams to the shader files
    auto vsStream = FileSystem::OpenStream(L"glsl/StaticGeometryVS.glsl");
    auto fsStream = FileSystem::OpenStream(L"glsl/TexturedPS.glsl");

    // Read the file contents into strings
    std::string vsSource((std::istreambuf_iterator<char>(*vsStream.get())), std::istreambuf_iterator<char>());
    std::string fsSource((std::istreambuf_iterator<char>(*fsStream.get())), std::istreambuf_iterator<char>());

    // Compile those strings into shaders
    _vertexShader = CompileShader(vsSource.c_str(), GL_VERTEX_SHADER);
    _fragmentShader = CompileShader(fsSource.c_str(), GL_FRAGMENT_SHADER);

    // Build our program from the shaders
    CHECK_GL(_program = glCreateProgram());
    CHECK_GL(glAttachShader(_program, _vertexShader));
    CHECK_GL(glAttachShader(_program, _fragmentShader));

    // Set the vertex attributes since our GlGeometry requires 0 = position, 1 = normal, and 2 = texcoord
    CHECK_GL(glBindAttribLocation(_program, 0, "inPosition"));
    CHECK_GL(glBindAttribLocation(_program, 1, "inNormal"));
    CHECK_GL(glBindAttribLocation(_program, 2, "inUV"));

    // Now we can link the program
    CHECK_GL(glLinkProgram(_program));

    // Check for a log and print it out if there is one
    GLsizei logSize = 0;
    CHECK_GL(glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &logSize));
    if (logSize > 0)
    {
        std::unique_ptr<GLchar[]> log(new GLchar[logSize]);
        CHECK_GL(glGetProgramInfoLog(_program, logSize, &logSize, log.get()));
        std::cout << "Program info log: " << std::endl << log.get() << std::endl;
    }

    // Check that the program linked properly
    GLint linkStatus = GL_FALSE;
    CHECK_GL(glGetProgramiv(_program, GL_LINK_STATUS, &linkStatus));
    CHECK_TRUE(linkStatus == GL_TRUE);

    // Retrieve the uniform locations
    GET_UNIFORM_LOCATION(_worldUniform, _program, "World");
    GET_UNIFORM_LOCATION(_viewProjUniform, _program, "ViewProjection"); 
    GET_UNIFORM_LOCATION(_lightUniforms[0].directionUniform, _program, "Light1.Direction");
    GET_UNIFORM_LOCATION(_lightUniforms[0].colorUniform, _program, "Light1.Color");
    GET_UNIFORM_LOCATION(_lightUniforms[1].directionUniform, _program, "Light2.Direction");
    GET_UNIFORM_LOCATION(_lightUniforms[1].colorUniform, _program, "Light2.Color");
    GET_UNIFORM_LOCATION(_lightUniforms[2].directionUniform, _program, "Light3.Direction");
    GET_UNIFORM_LOCATION(_lightUniforms[2].colorUniform, _program, "Light3.Color");
    GET_UNIFORM_LOCATION(_samplerUniform, _program, "Sampler");

    // Sampler always reads from texture slot 0
    CHECK_GL(glUseProgram(_program));
    CHECK_GL(glUniform1i(_samplerUniform, 0));
    CHECK_GL(glUseProgram(0));

    // Light1 (dim white light shining downward)
    SetLight(0, Vector3(0, 1, 0), Vector4(0.2f, 0.2f, 0.2f, 1));

    // Light2 (bright white light shining along forward direction)
    SetLight(1, Vector3(0, 0, -1), Vector4(1, 1, 1, 1));

    // Light3 (blue light shining upward from floor)
    SetLight(2, Vector3(0, -1, 0), Vector4(0, 0, 1, 1));

    // Create some default resources
    CreateQuad();
    CreateDefaultTexture();

    // Set up some default state
    CHECK_GL(glEnable(GL_CULL_FACE));
    CHECK_GL(glCullFace(GL_FRONT));
    CHECK_GL(glEnable(GL_BLEND));
    CHECK_GL(glEnable(GL_DEPTH_TEST));
}

void OpenGLGraphicsDevice::CleanUpBase()
{
    // Ensure we're using the correct context to clean up
    MakeContextCurrent();

    if (_vertexShader)
    {
        CHECK_GL(glDeleteShader(_vertexShader));
        _vertexShader = 0;
    }
    if (_fragmentShader)
    {
        CHECK_GL(glDeleteShader(_fragmentShader));
        _fragmentShader = 0;
    }

    // since resources hold weak references to the device, we need to break those off
    // before the context is destroyed
    for (auto i = 0; i < _boundTextures.size(); i++)
    {
        _boundTextures[i] = nullptr;
    }
    _boundGeometry = nullptr;
    _quad = nullptr;
    _defaultTexture = nullptr;
}

void OpenGLGraphicsDevice::CreateDefaultTexture()
{
    byte_t lightGreyPixel[] = { 190, 190, 190, 255 };
    _defaultTexture = GlTexture::Create(1, 1, TextureFormat::R8G8B8A8, lightGreyPixel);
}

void OpenGLGraphicsDevice::CreateQuad()
{
    // Create quad to use for 2D rendering
    Geometry::Vertex quadVertices[] =
    {
        Geometry::Vertex(Vector3(0.0f, 1.0f, 0.1f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 0.0f)),
        Geometry::Vertex(Vector3(1.0f, 1.0f, 0.1f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 0.0f)),
        Geometry::Vertex(Vector3(1.0f, 0.0f, 0.1f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 1.0f)),
        Geometry::Vertex(Vector3(0.0f, 0.0f, 0.1f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 1.0f)),
    };

    uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

    _quad = GlGeometry::Create(1, quadVertices, _countof(quadVertices), indices, _countof(indices));
}

_Use_decl_annotations_
void OpenGLGraphicsDevice::SetLight(uint32_t light, const Vector3& direction, const Vector4& color)
{
    CHECK_GL(glUseProgram(_program));
    CHECK_GL(glUniform3f(_lightUniforms[light].directionUniform, direction.x, direction.y, direction.z));
    CHECK_GL(glUniform4f(_lightUniforms[light].colorUniform, color.x, color.y, color.z, color.w));
    CHECK_GL(glUseProgram(0));
}

_Use_decl_annotations_
static GLuint CompileShader(const char* shaderSource, GLenum shaderType)
{
    // Create the shader
    CHECK_GL(GLuint shader = glCreateShader(shaderType));
    CHECK_GL(glShaderSource(shader, 1, &shaderSource, nullptr));
    CHECK_GL(glCompileShader(shader));

    // Check for a log and print it out if there is one
    GLsizei logSize = 0;
    CHECK_GL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize));
    if (logSize > 0)
    {
        std::unique_ptr<GLchar[]> log(new GLchar[logSize]);
        CHECK_GL(glGetShaderInfoLog(shader, logSize, &logSize, log.get()));
        std::cout << "Shader info log: " << std::endl << log.get() << std::endl;
    }

    // Check that the shader compiled properly
    GLint compileStatus = GL_FALSE;
    CHECK_GL(glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus));
    CHECK_TRUE(compileStatus == GL_TRUE);

    return shader;
}
#endif