#include "Effect.h"
#include "DirectX\DxGraphicsDevice.h"
#include "DirectX\DxEffect.h"

namespace GDK
{
    std::shared_ptr<Effect> Effect::Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ Type type)
    {
        if (static_cast<DxGraphicsDevice*>(graphicsDevice.get()) != nullptr)
        {
            return DxEffect::Create(graphicsDevice, type);
        }
        else
        {
            throw std::exception();
        }
    }

    Effect::Effect(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice) :
        _graphicsDevice(graphicsDevice)
    {
    }

    std::shared_ptr<GraphicsDevice> Effect::LockGraphicsDevice()
    {
        return _graphicsDevice.lock();
    }
}
