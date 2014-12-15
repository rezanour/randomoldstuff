#pragma once

#include "BaseRefCounted.h"
#include "IMessage.h"

namespace GDK
{
    class Message : public BaseRefCounted<IMessage>
    {
    public:
        // Construction/Destruction
        static HRESULT Create(MessageType type, uint16_t subType, size_t bodySize, Message** ppMessage);

        // IMessage
        const MessageHeader* GetHeader() const;
        void* GetBody();

    private:
        Message();
        ~Message();

        MessageHeader _header;
        void* _body;
    };
} // GDK
