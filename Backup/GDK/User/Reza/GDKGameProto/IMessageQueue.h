#pragma once

#include "IRefCounted.h"
#include "Message.h"

namespace GDK
{
    __interface IMessageQueue : public IRefCounted
    {
        HRESULT PostMessage(MessageHeader* message);
    };
} // GDK
