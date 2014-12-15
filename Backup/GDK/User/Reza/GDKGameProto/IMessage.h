#pragma once

#include "IRefCounted.h"

namespace GDK
{
    enum MessageType
    {
        MessageType_Task,
        MessageType_Input,
        MessageType_Renderer,
        MessageType_Audio,
    };

    struct MessageHeader
    {
        MessageType     Type;
        uint16_t        SubType;
        size_t          BodySize;
        uint64_t        TimeSent;
    };

    __interface IMessage : public IRefCounted
    {
        const MessageHeader* GetHeader() const;
        void* GetBody();
    };
} // GDK
