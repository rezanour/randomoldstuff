#ifdef __APPLE__

#include "Precomp.h"
#include "OpenGLGraphicsDeviceOSX.h"

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>

namespace GDK
{
namespace Graphics
{

class OpenGLGraphicsDeviceOSX::Impl
{
public:
    Impl(const GraphicsDeviceCreationParameters& parameters)
    {
        view = (NSView *)parameters.windowIdentity;

        NSOpenGLPixelFormatAttribute attrs[] =
        {
            NSOpenGLPFADoubleBuffer,
            0
        };
        NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
        context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
        [context makeCurrentContext];

// If the host is not using automatic reference counting, we must release the pixel format. The context
// will retain it as necessary.
#if (!__has_feature(objc_arc))
        [pixelFormat release];
#endif
    }

    ~Impl()
    {
// If the host is using automatic reference counting, we don't have to clean up the context
#if (!__has_feature(objc_arc))
        [context release];
#endif
    }

    void Present()
    {
        glFlush();
        [context flushBuffer];        
    }

    void MakeContextCurrent()
    {
        if (context.view != view)
        {
            context.view = view;
        }
        [context makeCurrentContext];
    }

private:
    NSView *view;
    NSOpenGLContext *context;
};

OpenGLGraphicsDeviceOSX::OpenGLGraphicsDeviceOSX(_In_ const GraphicsDeviceCreationParameters& parameters) :
    OpenGLGraphicsDevice(parameters),
    _impl(new Impl(parameters))
{
}

void GDKAPI OpenGLGraphicsDeviceOSX::Present()
{
    _impl->Present();
}

void GDKAPI OpenGLGraphicsDeviceOSX::MakeContextCurrent()
{
    _impl->MakeContextCurrent();
}

} // Graphics
} // GDK

#endif // __APPLE__