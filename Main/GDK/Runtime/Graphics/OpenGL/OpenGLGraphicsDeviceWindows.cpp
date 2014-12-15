#include "OpenGLGraphicsDeviceWindows.h"
#include <wrl\wrappers\corewrappers.h>
#include <GDKError.h>

using namespace GDK;
namespace Mwrlw = Microsoft::WRL::Wrappers;

_Use_decl_annotations_
OpenGLGraphicsDeviceWindows::OpenGLGraphicsDeviceWindows(const GraphicsDevice::CreateParameters& parameters) :
    OpenGLGraphicsDevice(parameters),
    _hdc(nullptr),
    _context(nullptr)
{
    HWND hwnd = static_cast<HWND>(parameters.windowIdentity);
    _hdc = GetDC(hwnd);

    //
    // Kind of silly, but we have a chicken and the egg problem here:
    // We need to initialize GLEW before we can create a 3.3+ context,
    // however we can't initialize GLEW until we have a context.... derp! :P
    //
    // So, let's create a dummy context, then init GLEW, then dump our context,
    // then init the real context we want.... like I said.... silly.
    //
    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    // Create the normal context
    SetPixelFormat(_hdc, ChoosePixelFormat(_hdc, &pfd), &pfd);
    _context = wglCreateContext(_hdc);
    CHECK_NOT_NULL(_context);
    
    wglMakeCurrent(_hdc, _context);
    
    // Initialize GLEW
    CHECK_GL(GLenum result = glewInit());
    CHECK_TRUE(result == GLEW_OK);
}

OpenGLGraphicsDeviceWindows::~OpenGLGraphicsDeviceWindows()
{
    CleanUpBase();

    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(_context);
    ReleaseDC(static_cast<HWND>(_parameters.windowIdentity), _hdc);
    _context = nullptr;
}

void OpenGLGraphicsDeviceWindows::Present()
{
    wglMakeCurrent(_hdc, _context);
    SwapBuffers(_hdc);
}

void OpenGLGraphicsDeviceWindows::MakeContextCurrent()
{
    wglMakeCurrent(_hdc, _context);
}
