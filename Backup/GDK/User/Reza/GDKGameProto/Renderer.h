#pragma once

#include "Devices.h"
#include "Message.h"

namespace GDK
{
    // specific messages for the renderer
    enum RendererMessageType
    {
        RendererMessage_CreateComponent,
        RendererMessage_DestroyComponent,
        RendererMessage_Render,
    };

    struct RendererMessageHeader
    {
        RendererMessageType     Type;
        uint16_t                SessionID;
    };
} // GDK
