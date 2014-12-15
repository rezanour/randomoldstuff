#pragma once

#include "Platform.h"
#include "RuntimeObject.h"
#include "GraphicsDevice.h"

namespace GDK
{
    class GraphicsDevice;

    class Texture : public RuntimeObject<Texture>
    {
    public:
        uint32_t GetWidth() const { return _width; }
        uint32_t GetHeight() const { return _height; }

    protected:
        Texture(_In_ uint32_t width, _In_ uint32_t height) : _width(width), _height(height) { }
        uint32_t _width;
        uint32_t _height;
    };
}
