#include <Platform.h>
#include <GraphicsDevice.h>
#include <GDKError.h>

#ifdef WIN32
#include "DirectX\DxGraphicsDevice.h"
#endif

namespace GDK
{
    std::shared_ptr<GraphicsDevice> GraphicsDevice::Create(_In_ const GraphicsDevice::CreateParameters& parameters)
    {
        switch (parameters.type)
        {
#ifdef WIN32
        case GraphicsDevice::Type::DirectX:
            return DxGraphicsDevice::Create(parameters);
#endif

        default:
            throw std::invalid_argument("Invalid GraphicsDevice::Type specified");
        }
    }

    GraphicsDevice::GraphicsDevice(_In_ const GraphicsDevice::CreateParameters& parameters) :
        _parameters(parameters)
    {
    }
}
