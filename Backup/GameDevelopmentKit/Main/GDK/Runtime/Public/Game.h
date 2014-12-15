#pragma once

#include "Platform.h"
#include "RuntimeObject.h"
#include "GraphicsDevice.h"

namespace GDK
{
    class Game : public RuntimeObject<Game>
    {
    public:
        struct CreateParameters
        {
            std::shared_ptr<GraphicsDevice> graphicsDevice;
        };

    protected:
        Game(_In_ const CreateParameters& parameters) 
        {
            UNREFERENCED_PARAMETER(parameters);
        }
    };
}
