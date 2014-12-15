#pragma once

#include <Platform.h>
#include "GraphicsDeviceBase.h"

namespace GDK
{
    class Effect : public RuntimeObject<Effect>
    {
    public:
        enum class Type
        {
            StaticGeometry3D,
            Draw2D,
        };

        enum class FilterType
        {
            Point,
            Linear
        };

        enum class AddressMode
        {
            Wrap,
            Clamp
        };

        enum class BlendMode
        {
            Opaque,
            AlphaBlend,
        };

        static std::shared_ptr<Effect> Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ Type type);

        virtual void Apply() = 0;
        virtual void Draw(_In_ const std::shared_ptr<Geometry>& geometry, _In_ const Matrix& world, _In_ uint32_t frame) = 0;

    protected:
        Effect(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice);

        std::shared_ptr<GraphicsDevice> LockGraphicsDevice();

    private:
        std::weak_ptr<GraphicsDevice> _graphicsDevice;
    };
}
