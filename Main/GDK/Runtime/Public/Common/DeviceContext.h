#pragma once

#include <Platform.h>

namespace GDK
{
    class GraphicsDevice;
    class AudioDevice;

    struct DeviceContext
    {
        DeviceContext()
        {}

        DeviceContext(_In_ const std::shared_ptr<GraphicsDevice>& graphics, _In_ const std::shared_ptr<AudioDevice>& audio) :
            graphicsDevice(graphics), audioDevice(audio)
        {}

        std::shared_ptr<GraphicsDevice> graphicsDevice;
        std::shared_ptr<AudioDevice> audioDevice;
    };
}
