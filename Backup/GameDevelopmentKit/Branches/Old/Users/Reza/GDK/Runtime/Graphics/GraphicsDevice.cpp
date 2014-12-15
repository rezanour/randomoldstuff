#include <GraphicsDevice.h>

#ifdef WIN32
#include "Dx11\Dx11GraphicsDevice.h"
#endif

namespace GDK
{
    std::shared_ptr<GraphicsDevice> GraphicsDevice::CreateDevice(_In_ Type type, _In_ const GraphicsDeviceCreationParameters& parameters)
    {
#ifdef WIN32
        if (type == Type::DirectX11)
        {
            return std::shared_ptr<GraphicsDevice>(new (__FILEW__, __LINE__) Graphics::Dx11GraphicsDevice(parameters));
        }
#endif

        throw std::invalid_argument("Unsupported graphics device requested.");
    }

    GraphicsDevice::GraphicsDevice(_In_ const GraphicsDeviceCreationParameters& parameters) :
        _parameters(parameters)
    {
    }
}
