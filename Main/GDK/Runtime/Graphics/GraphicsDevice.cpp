#include <Platform.h>
#include <GraphicsDevice.h>
#include <GDKError.h>

#include "GraphicsDeviceBase.h"

namespace GDK
{
    std::shared_ptr<GraphicsDevice> GraphicsDevice::Create(_In_ const GraphicsDevice::CreateParameters& parameters)
    {
        return GraphicsDeviceBase::Create(parameters);
    }

    GraphicsDevice::GraphicsDevice(_In_ const GraphicsDevice::CreateParameters& parameters) :
        _parameters(parameters)
    {
    }
}
