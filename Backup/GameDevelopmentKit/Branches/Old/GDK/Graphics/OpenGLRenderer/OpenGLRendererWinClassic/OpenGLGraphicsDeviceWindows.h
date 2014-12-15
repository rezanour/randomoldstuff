#ifdef WIN32

#pragma once

namespace GDK {
namespace Graphics {

    class OpenGLGraphicsDeviceWindows : public OpenGLGraphicsDevice
    {
    public:
        OpenGLGraphicsDeviceWindows(_In_ const GraphicsDeviceCreationParameters& parameters);
        ~OpenGLGraphicsDeviceWindows();

        virtual void GDKAPI Present();

    protected:
        virtual void GDKAPI MakeContextCurrent();

    private:
        HDC _hdc;
        HGLRC _context;
    };

} // Graphics
} // GDK

#endif // WIN32