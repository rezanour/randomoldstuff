#ifdef __APPLE__

#pragma once

namespace GDK {
namespace Graphics {

    class OpenGLGraphicsDeviceOSX : public OpenGLGraphicsDevice
    {
    public:
        OpenGLGraphicsDeviceOSX(_In_ const GraphicsDeviceCreationParameters& parameters);

        virtual void GDKAPI Present();
        
    protected:
        virtual void GDKAPI MakeContextCurrent();

    private:
        class Impl;
        std::unique_ptr<Impl> _impl;
    };

} // Graphics
} // GDK

#endif // __APPLE__