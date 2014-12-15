#pragma once
#ifdef WIN32

#include "OpenGLGraphicsDevice.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace GDK 
{
    class OpenGLGraphicsDeviceWindows : public OpenGLGraphicsDevice
    {
    public:
        OpenGLGraphicsDeviceWindows(_In_ const GraphicsDevice::CreateParameters& parameters);
        ~OpenGLGraphicsDeviceWindows();

        virtual void Present();

    protected:
        virtual void MakeContextCurrent();

    private:
        HDC _hdc;
        HGLRC _context;
    };
}

#endif // WIN32